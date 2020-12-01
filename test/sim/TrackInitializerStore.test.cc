//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerStore.test.cc
//---------------------------------------------------------------------------//
#include "sim/TrackInitializerStore.hh"

#include "celeritas_test.hh"
#include "geometry/GeoParams.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/base/SecondaryAllocatorStore.hh"
#include "physics/material/MaterialParams.hh"
#include "sim/ParamStore.hh"
#include "sim/StateStore.hh"
#include "sim/TrackInitializerStore.hh"
#include "TrackInitializerStore.test.hh"

namespace celeritas_test
{
using namespace celeritas;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class TrackInitTest : public celeritas::Test
{
  protected:
    void SetUp() override
    {
        num_tracks = 10;

        // Set up shared standard model particle data
        ParticleParams::VecAnnotatedDefs defs
            = {{{"gamma", pdg::gamma()},
                {zero_quantity(),
                 zero_quantity(),
                 ParticleDef::stable_decay_constant()}}};
        std::shared_ptr<ParticleParams> particle_params
            = std::make_shared<ParticleParams>(std::move(defs));

        // Set up shared material data
        MaterialParams::Input mats;
        mats.elements   = {{{1, units::AmuMass{1.008}, "H"}}};
        mats.materials  = {{{1e-5 * constants::na_avogadro,
                            100.0,
                            MatterState::gas,
                            {{ElementDefId{0}, 1.0}},
                            "H2"}}};
        std::shared_ptr<MaterialParams> material_params
            = std::make_shared<MaterialParams>(std::move(mats));

        // Set up shared geometry data
        std::string test_file
            = celeritas::Test::test_data_path("geometry", "twoBoxes.gdml");
        geo_params = std::make_shared<GeoParams>(test_file.c_str());

        // Get view to the shared problem data
        params = ParamStore(geo_params, material_params, particle_params);

        // Create primary particles
        primaries = DeviceVector<Primary>(num_tracks);
        std::vector<Primary> host_primaries;
        for (unsigned int i = 0; i < num_tracks; ++i)
        {
            host_primaries.push_back({ParticleDefId{0},
                                      units::MevEnergy{100. * (i + 1)},
                                      {0., 0., 0.},
                                      {0., 0., 1.},
                                      EventId{0},
                                      TrackId{i + 1}});
        }
        // Copy host-side primaries to device
        primaries.copy_to_device(make_span(host_primaries));
    }

    size_type                  num_tracks;
    std::shared_ptr<GeoParams> geo_params;
    ParamStore                 params;
    DeviceVector<Primary>      primaries;
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST_F(TrackInitTest, run)
{
    // Allocate storage on device for track state data
    StateStore::Input inp;
    inp.num_tracks = num_tracks;
    inp.geo        = geo_params;
    StateStore states(inp);

    // Allocate device data for secondary and track initializer storage
    SecondaryAllocatorStore secondaries(4 * num_tracks);
    TrackInitializerStore   track_init(states.size(), secondaries.capacity());

    // Check that all of the track slots were marked as empty
    std::vector<size_type> output_vacancies
        = vacancies_test(track_init.device_pointers());
    std::vector<size_type> expected_vacancies = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_VEC_EQ(expected_vacancies, output_vacancies);

    // Create track initializers on device from primary particles
    track_init.extend_from_primaries(primaries.device_pointers());

    // Check the energies of the track initializers created from primaries
    std::vector<double> output_initializers
        = initializers_test(track_init.device_pointers());
    std::vector<double> expected_initializers
        = {100., 200., 300., 400., 500., 600., 700., 800., 900., 1000.};
    EXPECT_VEC_SOFT_EQ(expected_initializers, output_initializers);

    // Initialize the primary tracks on device
    track_init.initialize_tracks(states, params);

    // Check the energies of the initialized tracks
    std::vector<double> output_tracks
        = tracks_test(states.device_pointers(), params.device_pointers());
    std::vector<double> expected_tracks
        = {1000., 900., 800., 700., 600., 500., 400., 300., 200., 100.};
    EXPECT_VEC_SOFT_EQ(expected_tracks, output_tracks);

    // Launch kernel to process interactions
    interact(states.device_pointers(),
             params.device_pointers(),
             secondaries.device_pointers());

    // Launch a kernel to create track initializers from
    // interactions/secondaries
    track_init.extend_from_secondaries(states, params);

    // Check the vacancies
    output_vacancies   = vacancies_test(track_init.device_pointers());
    expected_vacancies = {1, 4};
    EXPECT_VEC_EQ(expected_vacancies, output_vacancies);

    // Check the energies of the track initializers created from secondaries
    output_initializers   = initializers_test(track_init.device_pointers());
    expected_initializers = {8., 16., 7., 4., 8.};
    EXPECT_VEC_SOFT_EQ(expected_initializers, output_initializers);

    // Initialize secondaries on device
    track_init.initialize_tracks(states, params);

    // Check the energies of the initialized tracks
    output_tracks
        = tracks_test(states.device_pointers(), params.device_pointers());
    expected_tracks = {1000., 8., 800., 700., 4., 500., 400., 3., 200., 100.};
    EXPECT_VEC_SOFT_EQ(expected_tracks, output_tracks);
}

//---------------------------------------------------------------------------//
} // namespace celeritas_test

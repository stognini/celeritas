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
#include "physics/base/SecondaryAllocatorStore.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/material/MaterialParams.hh"
#include "sim/ParamPointers.hh"
#include "sim/StateStore.hh"
#include "sim/TrackInitializerStore.hh"
#include "TrackInitializerStore.test.hh"

namespace celeritas_test
{
using namespace celeritas;

//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//

ITTestInput::ITTestInput(std::vector<size_type>& host_alloc_size,
                         std::vector<int>&       host_alive)
    : alloc_size(host_alloc_size.size()), alive(host_alive.size())
{
    REQUIRE(host_alloc_size.size() == host_alive.size());
    alloc_size.copy_to_device(make_span(host_alloc_size));
    alive.copy_to_device(make_span(host_alive));
}

ITTestInputPointers ITTestInput::device_pointers()
{
    ITTestInputPointers result;
    result.alloc_size = alloc_size.device_pointers();
    result.alive      = alive.device_pointers();
    return result;
}

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class TrackInitTest : public celeritas::Test
{
  protected:
    void SetUp() override
    {
        // Set up shared geometry data
        std::string test_file
            = celeritas::Test::test_data_path("geometry", "twoBoxes.gdml");
        geo_params = std::make_shared<GeoParams>(test_file.c_str());

        // Set up shared material data
        MaterialParams::Input mats;
        mats.elements  = {{{1, units::AmuMass{1.008}, "H"}}};
        mats.materials = {{{1e-5 * constants::na_avogadro,
                            100.0,
                            MatterState::gas,
                            {{ElementDefId{0}, 1.0}},
                            "H2"}}};
        std::shared_ptr<MaterialParams> material_params
            = std::make_shared<MaterialParams>(std::move(mats));

        // Set up shared standard model particle data
        ParticleParams::VecAnnotatedDefs defs
            = {{{"gamma", pdg::gamma()},
                {zero_quantity(),
                 zero_quantity(),
                 ParticleDef::stable_decay_constant()}}};
        std::shared_ptr<ParticleParams> particle_params
            = std::make_shared<ParticleParams>(std::move(defs));

        // Set the shared problem data
        params = ParamStore(geo_params, material_params, particle_params);
    }

    // Create primary particles
    std::vector<Primary> generate_primaries(size_type num_primaries)
    {
        std::vector<Primary> result;
        for (unsigned int i = 0; i < num_primaries; ++i)
        {
            result.push_back({ParticleDefId{0},
                              units::MevEnergy{1. + i},
                              {0., 0., 0.},
                              {0., 0., 1.},
                              EventId{0},
                              TrackId{i}});
        }
        return result;
    }

    std::shared_ptr<GeoParams> geo_params;
    ParamStore                 params;
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST_F(TrackInitTest, run)
{
    constexpr size_type num_tracks = 10;
    constexpr size_type capacity   = 100;

    // Create 12 primary particles
    std::vector<Primary> primaries = generate_primaries(12);

    // Allocate storage on device
    StateStore              states({num_tracks, geo_params, 12345u});
    SecondaryAllocatorStore secondaries(capacity);
    TrackInitializerStore   track_init(num_tracks, capacity, primaries);

    // Check that all of the track slots were marked as empty
    ITTestOutput output, expected;
    output.vacancy   = vacancies_test(track_init.device_pointers());
    expected.vacancy = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_VEC_EQ(expected.vacancy, output.vacancy);

    // Create track initializers on device from primary particles
    track_init.extend_from_primaries();

    // Check the track IDs of the track initializers created from primaries
    output.initializer_id   = initializers_test(track_init.device_pointers());
    expected.initializer_id = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_VEC_EQ(expected.initializer_id, output.initializer_id);

    // Initialize the primary tracks on device
    track_init.initialize_tracks(states, params);

    // Check the IDs of the initialized tracks
    output.track_id   = tracks_test(states.device_pointers());
    expected.track_id = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
    EXPECT_VEC_EQ(expected.track_id, output.track_id);

    // Allocate input device data (number of secondaries to produce for each
    // track and whether the track survives the interaction)
    std::vector<size_type> alloc_size = {1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
    std::vector<int>       alive      = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    ITTestInput            input(alloc_size, alive);

    // Launch kernel to process interactions
    interact(states.device_pointers(),
             secondaries.device_pointers(),
             input.device_pointers());

    // Launch a kernel to create track initializers from secondaries
    track_init.extend_from_secondaries(states, params);

    // Check the vacancies
    output.vacancy   = vacancies_test(track_init.device_pointers());
    expected.vacancy = {2, 6};
    EXPECT_VEC_EQ(expected.vacancy, output.vacancy);

    // Check the track IDs of the track initializers created from secondaries
    output.initializer_id   = initializers_test(track_init.device_pointers());
    expected.initializer_id = {0, 1, 15, 16, 17};
    EXPECT_VEC_EQ(expected.initializer_id, output.initializer_id);

    // Initialize secondaries on device
    track_init.initialize_tracks(states, params);

    // Check the track IDs of the initialized tracks
    output.track_id   = tracks_test(states.device_pointers());
    expected.track_id = {12, 10, 17, 8, 13, 6, 16, 4, 14, 2};
    EXPECT_VEC_EQ(expected.track_id, output.track_id);
}

//---------------------------------------------------------------------------//
} // namespace celeritas_test

//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/executor/MuPairProductionExecutor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Assert.hh"
#include "corecel/Macros.hh"
#include "celeritas/em/data/MuPairProductionData.hh"
#include "celeritas/em/interactor/MuPairProductionInteractor.hh"
#include "celeritas/geo/GeoTrackView.hh"
#include "celeritas/global/CoreTrackView.hh"
#include "celeritas/mat/MaterialTrackView.hh"
#include "celeritas/phys/Interaction.hh"
#include "celeritas/phys/PhysicsStepView.hh"
#include "celeritas/random/RngEngine.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
struct MuPairProductionExecutor
{
    inline CELER_FUNCTION Interaction
    operator()(celeritas::CoreTrackView const& track);

    NativeCRef<MuPairProductionData> params;
};

//---------------------------------------------------------------------------//
/*!
 * Sample muon pair production from the current track.
 */
CELER_FUNCTION Interaction
MuPairProductionExecutor::operator()(CoreTrackView const& track)
{
    auto cutoff = track.make_cutoff_view();
    auto particle = track.make_particle_view();
    auto elcomp_id = track.make_physics_step_view().element();
    CELER_ASSERT(elcomp_id);
    auto element
        = track.make_material_view().make_material_view().make_element_view(
            elcomp_id);
    auto allocate_secondaries
        = track.make_physics_step_view().make_secondary_allocator();
    auto const& dir = track.make_geo_view().dir();

    MuPairProductionInteractor interact(
        params, particle, cutoff, element, dir, allocate_secondaries);

    auto rng = track.make_rng_engine();
    return interact(rng);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

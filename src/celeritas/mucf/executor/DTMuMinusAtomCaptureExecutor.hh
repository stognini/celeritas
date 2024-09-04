//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/executor/DTMuMinusAtomCaptureExecutor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Macros.hh"
#include "celeritas/global/CoreTrackView.hh"
#include "celeritas/mat/ElementSelector.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
struct DTMuMinusAtomCaptureExecutor
{
    inline CELER_FUNCTION Interaction
    operator()(celeritas::CoreTrackView const& track);

    DTMuMinusAtomCaptureData model_data;
};

//---------------------------------------------------------------------------//
/*!
 * Sample a deuterium or tritium capture for the current track.
 */
CELER_FUNCTION Interaction
DTMuMinusAtomCaptureExecutor::operator()(celeritas::CoreTrackView const& track)
{
    auto allocate_secondaries
        = track.make_physics_step_view().make_secondary_allocator();
    auto particle = track.make_particle_view();
    auto material = track.make_material_view().make_material_view();

    auto elcomp_id = track.make_physics_step_view().element();
    if (!elcomp_id)
    {
        // Sample an element
    }

    auto elem_id = material.element_id(elcomp_id);
    CELER_ASSERT(elem_id == model_data.deuterium
                 || elem_id == model_data.tritium);
    auto element = material.make_element_view(elcomp_id);

    DTMuMinusAtomCaptureInteractor interact(
        model_data, particle, material, element, allocate_secondaries);
    auto rng = track.make_rng_engine();
    return interact(rng);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

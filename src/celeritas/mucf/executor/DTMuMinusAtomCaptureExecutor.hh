//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/executor/DTMuMinusAtomCaptureExecutor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Assert.hh"
#include "celeritas/global/CoreTrackView.hh"
#include "celeritas/mat/ElementView.hh"
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
 * Sample a deuteron or triton capture for the current track.
 */
CELER_FUNCTION Interaction
DTMuMinusAtomCaptureExecutor::operator()(celeritas::CoreTrackView const& track)
{
    auto phys_step_view = track.physics_step();
    auto elcomp_id = phys_step_view.element();
    CELER_ASSERT(elcomp_id);

    auto material = track.material().material_record();
    auto element = material.element_record(elcomp_id);
    CELER_ASSERT(element.atomic_number() == AtomicNumber{1});  // Must be H

    auto rng = track.rng();
    auto allocate_secondaries = phys_step_view.make_secondary_allocator();
    DTMuMinusAtomCaptureInteractor interact(
        model_data, track.make_particle_view(), element, allocate_secondaries);
    return interact(rng);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

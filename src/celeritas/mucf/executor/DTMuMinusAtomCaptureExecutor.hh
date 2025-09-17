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
#include "celeritas/mat/IsotopeView.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh"
#include "celeritas/random/ElementSelector.hh"
#include "celeritas/random/IsotopeSelector.hh"

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
    auto rng = track.rng();

    auto phys_track_view = track.physics();

    auto phys_step_view = track.physics_step();
    auto elcomp_id = phys_step_view.element();
#if 0
    if (!elcomp_id)
    {
        // Sample an element; FIX ME
        auto model_id = phys_track_view.make_model_finder(ParticleProcessId{0});
        auto select_element = track.physics().make_element_selector(
            phys_track_view.cdf_table(model_id), energy);
        elcomp_id = select_element(rng);
        CELER_ASSERT(elcomp_id);
        // Set element
        phys_step_view.element(elcomp_id);
    }
#endif

    auto material = track.material().material_record();
    ElementView element_view = material.element_record(elcomp_id);
    IsotopeSelector select_isotope(element_view);
    IsotopeView target = element_view.isotope_record(select_isotope(rng));
    auto const target_id = target.isotope_id();
    CELER_ASSERT(target_id == model_data.deuteron
                 || target_id == model_data.triton);

    auto allocate_secondaries = phys_step_view.make_secondary_allocator();
    DTMuMinusAtomCaptureInteractor interact(model_data,
                                            track.make_particle_view(),
                                            material,
                                            element_view,
                                            allocate_secondaries);
    return interact(rng);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

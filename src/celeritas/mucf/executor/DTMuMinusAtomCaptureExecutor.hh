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
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
struct DTMuMinusAtomCaptureExecutor
{
    inline CELER_FUNCTION Interaction
    operator()(celeritas::CoreTrackView const& track);

    DTMuMinusAtomCaptureData params;
};

//---------------------------------------------------------------------------//
/*!
 * Sample a deuterium or tritium capture for the current track.
 */
CELER_FUNCTION Interaction
DTMuMinusAtomCaptureExecutor::operator()(celeritas::CoreTrackView const& track)
{
    auto particle = track.make_particle_view();
    auto const& dir = track.make_geo_view().dir();

    DTMuMinusAtomCaptureInteractor interact(params, particle);
    auto rng = track.make_rng_engine();
    return interact(rng);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

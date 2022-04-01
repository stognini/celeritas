//----------------------------------*-C++-*----------------------------------//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RayleighLauncher.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Assert.hh"
#include "base/Macros.hh"
#include "sim/CoreTrackView.hh"

#include "RayleighData.hh"
#include "RayleighInteractor.hh"

namespace celeritas
{
namespace detail
{
//---------------------------------------------------------------------------//
/*!
 * Apply Rayleigh to the current track.
 */
inline CELER_FUNCTION Interaction
rayleigh_interact_track(RayleighRef const& model, CoreTrackView const& track)
{
    auto        particle = track.make_particle_view();
    const auto& dir      = track.make_geo_view().dir();

    // Assume only a single element in the material, for now
    CELER_ASSERT(track.make_material_view().make_material_view().num_elements()
                 == 1);
    ElementId el_id{0};

    RayleighInteractor interact(model, particle, dir, el_id);
    auto               rng = track.make_rng_engine();
    return interact(rng);
}

//---------------------------------------------------------------------------//
} // namespace detail
} // namespace celeritas

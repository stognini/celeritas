//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerPointers.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Types.hh"
#include "geometry/GeoStatePointers.hh"
#include "physics/base/ParticleStatePointers.hh"
#include "physics/base/Primary.hh"
#include "SimStatePointers.hh"
#include "Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Lightweight version of a track used to initialize new tracks from primaries
 * or secondaries
 */
struct TrackInitializer
{
    // Simulation state
    SimTrackState       sim;
    GeoStateInitializer geo;
    ParticleTrackState  particle;

    // Initialize from a primary particle
    CELER_FUNCTION TrackInitializer& operator=(const Primary& primary)
    {
        particle.def_id = primary.def_id;
        particle.energy = primary.energy;
        geo.dir         = primary.direction;
        geo.pos         = primary.position;
        sim.event_id    = primary.event_id;
        sim.track_id    = primary.track_id;
        sim.parent_id   = TrackId{};
        sim.alive       = true;
        return *this;
    }
};

//---------------------------------------------------------------------------//
/*!
 * View to the data used to initialize new tracks.
 */
struct TrackInitializerPointers
{
    span<TrackInitializer>    initializers;
    span<size_type>           parent;
    span<size_type>           vacancies;
    span<size_type>           secondary_counts;
    span<TrackId::value_type> track_counter;

    explicit CELER_FUNCTION operator bool() const
    {
        return !secondary_counts.empty();
    }
};

//---------------------------------------------------------------------------//
} // namespace celeritas

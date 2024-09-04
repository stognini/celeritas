//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Macros.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/phys/Interaction.hh"
#include "celeritas/phys/ParticleTrackView.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Brief class description.
 *
 */
class DTMuMinusAtomCaptureInteractor
{
  public:
    //!@{
    //! \name Type aliases
    //!@}

  public:
    // Construct with defaults
    inline CELER_FUNCTION
    DTMuMinusAtomCaptureInteractor(DTMuMinusAtomCaptureData const& shared,
                                   ParticleTrackView const& particle);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

  private:
    // Shared constant physics properties
    DTMuMinusAtomCaptureData const& shared_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Construct with shared and state data.
 */
DTMuMinusAtomCaptureInteractor::DTMuMinusAtomCaptureInteractor(
    DTMuMinusAtomCaptureData const& shared, ParticleTrackView const& particle)
    : shared_(shared)
{
    CELER_EXPECT(particle.particle_id() == shared_.muon);
}

//---------------------------------------------------------------------------//
/*!
 *
 */
template<class Engine>
CELER_FUNCTION Interaction DTMuMinusAtomCaptureInteractor::operator()(Engine& rng)
{
    // INTERACT
}
//---------------------------------------------------------------------------//
}  // namespace celeritas

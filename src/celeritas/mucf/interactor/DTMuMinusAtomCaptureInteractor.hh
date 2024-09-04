//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/interactor/DTMuMinusAtomCaptureInteractor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Macros.hh"
#include "corecel/data/StackAllocator.hh"
#include "celeritas/mat/ElementView.hh"
#include "celeritas/mat/MaterialView.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/phys/Interaction.hh"
#include "celeritas/phys/ParticleTrackView.hh"
#include "celeritas/phys/Secondary.hh"

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
    // Construct with defaults
    inline CELER_FUNCTION
    DTMuMinusAtomCaptureInteractor(DTMuMinusAtomCaptureData const& shared,
                                   ParticleTrackView const& particle,
                                   MaterialView const& material,
                                   ElementView const& element,
                                   StackAllocator<Secondary>& allocate);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

  private:
    // Shared constant physics properties
    DTMuMinusAtomCaptureData const& shared_;
    // Material properties
    MaterialView const& material_;
    // Element properties
    ElementView const& element_;
    // Allocate space for secondary particle (one muonic d or t)
    StackAllocator<Secondary>& allocate_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Construct with shared and state data.
 */
DTMuMinusAtomCaptureInteractor::DTMuMinusAtomCaptureInteractor(
    DTMuMinusAtomCaptureData const& shared,
    ParticleTrackView const& particle,
    MaterialView const& material,
    ElementView const& element,
    StackAllocator<Secondary>& allocate)
    : shared_(shared)
    , material_(material)
    , element_(element)
    , allocate_(allocate)
{
    CELER_EXPECT(particle.particle_id() == shared_.muon);
}

//---------------------------------------------------------------------------//
/*!
 * Sample a muon capture by a deuterium or tritium in the material. The final
 * muonic atom is also at rest.
 */
template<class Engine>
CELER_FUNCTION Interaction DTMuMinusAtomCaptureInteractor::operator()(Engine& rng)
{
    // Allocate space for the final muonic d or t atom
    Secondary* secondary = allocate_(1);
    if (secondaries == nullptr)
    {
        // Failed to allocate space for two secondaries
        return Interaction::from_failure();
    }

    Interaction result = Interaction::from_absorption();
    result.secondaries = {secondary};

    // TODO: implement AtRestDoIt

    return result;
}
//---------------------------------------------------------------------------//
}  // namespace celeritas

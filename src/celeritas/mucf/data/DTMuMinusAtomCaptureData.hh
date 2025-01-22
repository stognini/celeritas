//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/data/DTMuMinusAtomCaptureData.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Macros.hh"
#include "celeritas/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Data for creating a \c DTMuMinusAtomCaptureInteractor .
 */
struct DTMuMinusAtomCaptureData
{
    ElementId hydrogen;  //!< Hydrogen with d and t isotopes
    IsotopeId deuteron;
    IsotopeId triton;
    ParticleId muon;
    ParticleId muonic_deuteron;
    ParticleId muonic_triton;

    //! Check whether the data is assigned
    explicit CELER_FUNCTION operator bool() const
    {
        return hydrogen && deuteron && triton && muon && muonic_deuteron
               && muonic_triton;
    }
};

using DTMuMinusCaptureHostRef = DTMuMinusAtomCaptureData;
using DTMuMinusCaptureDeviceRef = DTMuMinusAtomCaptureData;

//---------------------------------------------------------------------------//
}  // namespace celeritas

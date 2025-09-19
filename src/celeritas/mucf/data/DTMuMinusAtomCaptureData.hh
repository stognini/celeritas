//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
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
    ParticleId muonic_deuteron_spin_3_over_2;
    ParticleId muonic_deuteron_spin_1_over_2;
    ParticleId muonic_triton_spin_1;
    ParticleId muonic_triton_spin_0;

    //! Check whether the data is assigned
    explicit CELER_FUNCTION operator bool() const
    {
        return hydrogen && deuteron && triton && muon
               && muonic_deuteron_spin_3_over_2
               && muonic_deuteron_spin_1_over_2 && muonic_triton_spin_1
               && muonic_triton_spin_0;
    }

    //! Fraction of muonic d with spin = 3/2; remainder is for spin = 1/2
    static constexpr real_type muonic_deuteron_spin_fraction()
    {
        return real_type{2 / 3};
    }

    //! Fraction of muonic t with spin = 1; remainder is for spin = 0
    static constexpr real_type muonic_triton_spin_fraction() { return 0.75; }
};

using DTMuMinusCaptureHostRef = DTMuMinusAtomCaptureData;
using DTMuMinusCaptureDeviceRef = DTMuMinusAtomCaptureData;

//---------------------------------------------------------------------------//
}  // namespace celeritas

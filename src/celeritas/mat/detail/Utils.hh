//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mat/detail/Utils.hh
//! \brief Helper functions for constructing materials
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Types.hh"
#include "celeritas/Quantities.hh"
#include "celeritas/phys/AtomicNumber.hh"

namespace celeritas
{
struct ElementRecord;

namespace detail
{
//---------------------------------------------------------------------------//
double calc_coulomb_correction(AtomicNumber atomic_number);
double calc_mass_rad_coeff(ElementRecord const& el);
units::MevEnergy get_mean_excitation_energy(AtomicNumber atomic_number);

//---------------------------------------------------------------------------//
}  // namespace detail
}  // namespace celeritas

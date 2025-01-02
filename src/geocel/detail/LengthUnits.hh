//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geocel/detail/LengthUnits.hh
//! \brief NOTE: only use inside geocel; prefer celeritas/Units.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Config.hh"

#include "corecel/Types.hh"
#include "corecel/math/Constant.hh"

namespace celeritas
{
namespace lengthunits
{
//---------------------------------------------------------------------------//
#define CELER_ICRT inline constexpr Constant

#if CELERITAS_UNITS == CELERITAS_UNITS_CGS
CELER_ICRT meter{100};
CELER_ICRT centimeter{1};
CELER_ICRT millimeter{0.1};
#elif CELERITAS_UNITS == CELERITAS_UNITS_SI
CELER_ICRT meter{1};
CELER_ICRT centimeter{0.01};
CELER_ICRT millimeter{0.001};
#elif CELERITAS_UNITS == CELERITAS_UNITS_CLHEP
CELER_ICRT meter{1000};
CELER_ICRT centimeter{10};
CELER_ICRT millimeter{1};
#else
#    error "CELERITAS_UNITS is undefined"
#endif

#undef CELER_ICRT
//---------------------------------------------------------------------------//
}  // namespace lengthunits
}  // namespace celeritas

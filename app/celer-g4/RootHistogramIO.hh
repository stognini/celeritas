//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/RootHistogramIO.hh
//---------------------------------------------------------------------------//
#pragma once

namespace celeritas
{
namespace app
{
//---------------------------------------------------------------------------//
/*!
 * ROOT histogram binning data.
 */
struct RootHistogramDef
{
    std::size_t nbins{0};
    double min{0};
    double max{0};

    explicit operator bool() const { return nbins > 0; }
};

//---------------------------------------------------------------------------//
/*!
 * List of ROOT histograms.
 */
struct RootHistograms
{
    RootHistogramDef energy_dep;  //!< SD energy deposition [MeV]
    RootHistogramDef time;  //!< SD pre-step global time [s]

    explicit operator bool() const { return energy_dep && time; }
};

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

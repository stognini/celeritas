//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/RootHistogramData.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/ext/RootUniquePtr.hh"

class TH1D;

namespace celeritas
{
namespace app
{
//---------------------------------------------------------------------------//
/*!
 * ROOT histogram binning data.
 */
struct RootHistogramInputDef
{
    std::size_t nbins{0};
    double min{0};
    double max{0};

    explicit operator bool() const { return nbins > 0; }
};

//---------------------------------------------------------------------------//
/*!
 * ROOT histogram definitions input.
 */
struct RootHistogramInput
{
    RootHistogramInputDef energy_dep;  //!< SD energy deposition [MeV]
    RootHistogramInputDef time;  //!< SD pre-step global time [s]
    RootHistogramInputDef step_len;  //!< SD step length [cm]

    explicit operator bool() const { return energy_dep && time && step_len; }
};

//---------------------------------------------------------------------------//
/*!
 * Histogram objects written to the ROOT file.
 * TODO: update to UPExtern when RootIO is overhauled.
 */
struct RootHistograms
{
    TH1D* energy_dep;
    TH1D* time;
    TH1D* step_len;
};

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

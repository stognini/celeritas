//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/RootHistogramIO.json.cc
//---------------------------------------------------------------------------//
#include "RootHistogramIO.json.hh"

namespace celeritas
{
namespace app
{
//---------------------------------------------------------------------------//
void from_json(nlohmann::json const& j, RootHistogramDef& value)
{
    value.nbins = j.at("nbins").get<std::size_t>();
    value.min = j.at("min").get<double>();
    value.max = j.at("max").get<double>();
}

//---------------------------------------------------------------------------//
void from_json(nlohmann::json const& j, RootHistograms& value)
{
#define RHIO_INPUT(NAME) j.at(#NAME).get_to(value.NAME);

    RHIO_INPUT(energy_dep);
    RHIO_INPUT(time);

#undef RHIO_INPUT
}

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

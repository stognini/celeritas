//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/RootHistogramIO.json.hh
//---------------------------------------------------------------------------//
#pragma once

#include <nlohmann/json.hpp>

#include "RootHistogramIO.hh"

namespace celeritas
{
namespace app
{
//---------------------------------------------------------------------------//

void from_json(nlohmann::json const& j, RootHistogramDef& value);
void from_json(nlohmann::json const& j, RootHistograms& value);

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

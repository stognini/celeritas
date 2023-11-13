//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/RootHistogramData.json.hh
//---------------------------------------------------------------------------//
#pragma once

#include <nlohmann/json.hpp>

#include "RootHistogramData.hh"

namespace celeritas
{
namespace app
{
//---------------------------------------------------------------------------//

void from_json(nlohmann::json const& j, RootHistogramInputDef& value);
void from_json(nlohmann::json const& j, RootHistogramInput& value);

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

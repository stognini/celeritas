//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geocel/g4/GeantGeoTestBase.hh
//---------------------------------------------------------------------------//
#pragma once

#include "geocel/GenericGeoTestBase.hh"
#include "geocel/g4/GeantGeoData.hh"
#include "geocel/g4/GeantGeoParams.hh"
#include "geocel/g4/GeantGeoTrackView.hh"
#include "geocel/g4/GeantGeoTraits.hh"

namespace celeritas
{
namespace test
{
//---------------------------------------------------------------------------//
using GeantGeoTestBase = GenericGeoTestBase<GeantGeoParams>;

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace celeritas

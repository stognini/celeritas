//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/LArSphereBase.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/ext/GeantImporter.hh"
#include "celeritas/phys/ProcessBuilder.hh"

#include "GeantTestBase.hh"

namespace celeritas
{
namespace test
{
//---------------------------------------------------------------------------//
/*!
 * Test harness for liquid argon sphere with optical properties.
 *
 * This class requires Geant4 to import the data. MSC is on by default.
 */
class LArSphereBase : public GeantTestBase
{
  protected:
    std::string_view geometry_basename() const override
    {
        return "lar-sphere";
    }

    ProcessBuilderOptions build_process_options() const override
    {
        auto result = GeantTestBase::build_process_options();
        return result;
    }

    GeantImportDataSelection build_import_data_selection() const override
    {
        auto result = GeantTestBase::build_import_data_selection();
        result.processes |= GeantImportDataSelection::optical;
        return result;
    }
};

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace celeritas

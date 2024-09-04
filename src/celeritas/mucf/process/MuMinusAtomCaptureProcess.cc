//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/process/MuMinusAtomCaptureProcess.cc
//---------------------------------------------------------------------------//
#include "MuMinusAtomCaptureProcess.hh"

#include <memory>

#include "celeritas/grid/ValueGridBuilder.hh"
#include "celeritas/grid/ValueGridType.hh"
#include "celeritas/mucf/model/DTMuMinusAtomCaptureModel.hh"
#include "celeritas/phys/Model.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from host data.
 */
MuMinusAtomCaptureProcess::MuMinusAtomCaptureProcess()
{
    // TBD
}

//---------------------------------------------------------------------------//
/*!
 * Construct the models associated with this process.
 */
auto MuMinusAtomCaptureProcess::build_models(ActionIdIter start_id) const
    -> VecModel
{
    return {std::make_shared<DTMuMinusAtomCaptureModel>(*start_id++)};
}

//---------------------------------------------------------------------------//
/*!
 * Get the interaction cross sections for the given energy range.
 */
auto MuMinusAtomCaptureProcess::step_limits(Applicability range) const
    -> StepLimitBuilders
{
    CELER_EXPECT(range.particle == muon_id_);

    StepLimitBuilders builders;
    builders[ValueGridType::macro_xs] = std::make_unique<ValueGridOTFBuilder>();

    return builders;
}

//---------------------------------------------------------------------------//
/*!
 * Name of the process.
 */
std::string_view MuMinusAtomCaptureProcess::label() const
{
    return "Atomic capture of a negative muon";
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

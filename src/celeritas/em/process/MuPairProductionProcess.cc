//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/process/MuPairProductionProcess.cc
//---------------------------------------------------------------------------//
#include "MuPairProductionProcess.hh"

#include <utility>

#include "corecel/Assert.hh"
#include "corecel/cont/Range.hh"
#include "celeritas/em/model/MuPairProductionModel.hh"
#include "celeritas/io/ImportProcess.hh"
#include "celeritas/phys/PDGNumber.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from host data.
 */
MuPairProductionProcess::MuPairProductionProcess(SPConstParticles particles,
                                                 SPConstImported process_data,
                                                 Options options,
                                                 SPConstImportTable table)
    : particles_(std::move(particles))
    , imported_(process_data,
                particles_,
                ImportProcessClass::mu_pair_prod,
                {pdg::mu_minus(), pdg::mu_plus()})
    , options_(options)
    , table_(std::move(table))
{
    CELER_EXPECT(particles_);
    CELER_EXPECT(table_);
}

//---------------------------------------------------------------------------//
/*!
 * Construct the models associated with this process.
 */
auto MuPairProductionProcess::build_models(ActionIdIter start_id) const
    -> VecModel
{
    return {std::make_shared<MuPairProductionModel>(
        *start_id++, *particles_, imported_.processes(), *table_)};
}

//---------------------------------------------------------------------------//
/*!
 * Get the interaction cross sections for the given energy range.
 */
auto MuPairProductionProcess::step_limits(Applicability applic) const
    -> StepLimitBuilders
{
    return imported_.step_limits(std::move(applic));
}

//---------------------------------------------------------------------------//
/*!
 * Name of the process.
 */
std::string_view MuPairProductionProcess::label() const
{
    return "Muon electron-positron pair production";
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

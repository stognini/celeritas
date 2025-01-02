//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/process/MuPairProductionProcess.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>

#include "celeritas/io/ImportMuPairProductionTable.hh"
#include "celeritas/phys/Applicability.hh"
#include "celeritas/phys/AtomicNumber.hh"
#include "celeritas/phys/ImportedProcessAdapter.hh"
#include "celeritas/phys/ParticleParams.hh"
#include "celeritas/phys/Process.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Electron-positron pair production process for muons.
 */
class MuPairProductionProcess : public Process
{
  public:
    //!@{
    //! \name Type aliases
    using SPConstParticles = std::shared_ptr<ParticleParams const>;
    using SPConstImported = std::shared_ptr<ImportedProcesses const>;
    using SPConstImportTable
        = std::shared_ptr<ImportMuPairProductionTable const>;
    //!@}

    // Options for the pair production process
    struct Options
    {
        bool use_integral_xs{true};  //!> Use integral method for sampling
                                     //! discrete interaction length
    };

  public:
    // Construct from pair production data
    MuPairProductionProcess(SPConstParticles particles,
                            SPConstImported process_data,
                            Options options,
                            SPConstImportTable table);

    // Construct the models associated with this process
    VecModel build_models(ActionIdIter start_id) const final;

    // Get the interaction cross sections for the given energy range
    StepLimitBuilders step_limits(Applicability range) const final;

    //! Whether to use the integral method to sample interaction length
    bool use_integral_xs() const final { return options_.use_integral_xs; }

    // Name of the process
    std::string_view label() const final;

  private:
    SPConstParticles particles_;
    ImportedProcessAdapter imported_;
    Options options_;
    SPConstImportTable table_;
};

//---------------------------------------------------------------------------//
}  // namespace celeritas

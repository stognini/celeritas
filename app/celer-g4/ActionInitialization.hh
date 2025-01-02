//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celer-g4/ActionInitialization.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <G4VUserActionInitialization.hh>

#include "accel/SharedParams.hh"

namespace celeritas
{
class HepMC3PrimaryGenerator;

namespace app
{
class GeantDiagnostics;
//---------------------------------------------------------------------------//
/*!
 * Set up demo-specific action initializations.
 */
class ActionInitialization final : public G4VUserActionInitialization
{
  public:
    //!@{
    //! \name Type aliases
    using SPParams = std::shared_ptr<SharedParams>;
    //!@}

  public:
    explicit ActionInitialization(SPParams params);
    void BuildForMaster() const final;
    void Build() const final;

    //! Get the number of events to be transported
    int num_events() const { return num_events_; }

  private:
    SPParams params_;
    std::shared_ptr<GeantDiagnostics> diagnostics_;
    std::shared_ptr<HepMC3PrimaryGenerator> hepmc_gen_;
    int num_events_{0};
    mutable bool init_shared_{true};
};

//---------------------------------------------------------------------------//
}  // namespace app
}  // namespace celeritas

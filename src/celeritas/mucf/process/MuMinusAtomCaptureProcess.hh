//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/process/MuMinusAtomCaptureProcess.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/Types.hh"
#include "celeritas/mat/MaterialParams.hh"
#include "celeritas/phys/Process.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Muon capture by a given atom in the material.
 * Different atoms (or set of) should be managed by different models.
 */
class MuMinusAtomCaptureProcess final : public Process
{
  public:
    //!@{
    //! \name Type aliases
    using SPConstParticles = std::shared_ptr<ParticleParams const>;
    using SPConstMaterials = std::shared_ptr<MaterialParams const>;
    //!@}

  public:
    // Construct from particle data
    explicit MuMinusAtomCaptureProcess(SPConstParticles particles,
                                       SPConstMaterials materials);

    // Construct the models associated with this process
    VecModel build_models(ActionIdIter start_id) const final;

    // Get the interaction cross sections for the given energy range
    StepLimitBuilders step_limits(Applicability applicability) const final;

    // Whether to use the integral method to sample interaction length
    bool use_integral_xs() const final { return false; }

    // Name of the process
    std::string_view label() const final;

  private:
    SPConstParticles particles_;
    SPConstMaterials materials_;
    ParticleId muon_id_;
};

//---------------------------------------------------------------------------//
}  // namespace celeritas
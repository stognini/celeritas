//----------------------------------*-C++-*----------------------------------//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/model/MollerBhabhaModel.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/em/data/MollerBhabhaData.hh"
#include "celeritas/phys/Model.hh"

namespace celeritas
{
class ParticleParams;

//---------------------------------------------------------------------------//
/*!
 * Set up and launch the Moller-Bhabha model interaction.
 */
class MollerBhabhaModel final : public Model
{
  public:
    // Construct from model ID and other necessary data
    MollerBhabhaModel(ActionId id, const ParticleParams& particles);

    // Particle types and energy ranges that this model applies to
    SetApplicability applicability() const final;

    // Apply the interaction kernel on host
    void execute(CoreHostRef const&) const final;

    // Apply the interaction kernel on device
    void execute(CoreDeviceRef const&) const final;

    // ID of the model
    ActionId action_id() const final;

    //! Short name for the interaction kernel
    std::string label() const final { return "ioni-moller-bhabha"; }

    //! Name of the model, for user interaction
    std::string description() const final
    {
        return "Moller+Bhabha scattering";
    }

  private:
    MollerBhabhaData interface_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
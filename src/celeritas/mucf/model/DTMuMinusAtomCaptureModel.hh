//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/model/DTMuMinusAtomCaptureModel.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/mat/MaterialParams.hh"
#include "celeritas/mucf/data/DTMuMinusAtomCaptureData.hh"
#include "celeritas/mucf/executor/DTMuMinusAtomCaptureExecutor.hh"  // IWYU pragma: associated
#include "celeritas/phys/InteractionApplier.hh"  // IWYU pragma: associated
#include "celeritas/phys/Model.hh"
#include "celeritas/phys/ParticleParams.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Muon capture by a deuterium or tritium atom in the material.
 */
class DTMuMinusAtomCaptureModel final : public Model, public ConcreteAction
{
  public:
    // Construct with defaults
    inline DTMuMinusAtomCaptureModel(ActionId id,
                                     ParticleParams const& particles,
                                     MaterialParams const& materials);

    // Particle types and energy ranges that this model applies to
    SetApplicability applicability() const final;

    // Get the microscopic cross sections for the given particle and material
    MicroXsBuilders micro_xs(Applicability) const final;

    // Apply the interaction kernel on host
    void step(CoreParams const&, CoreStateHost&) const final;

    // Apply the interaction kernel on device
    void step(CoreParams const&, CoreStateDevice&) const final;

    //!@{
    //! Access model data
    DTMuMinusAtomCaptureData const& host_ref() const { return data_; }
    DTMuMinusAtomCaptureData const& device_ref() const { return data_; }
    //!@}

  private:
    DTMuMinusAtomCaptureData data_;
};

//---------------------------------------------------------------------------//
}  // namespace celeritas

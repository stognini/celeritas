//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/model/DTMuMinusAtomCaptureModel.cc
//---------------------------------------------------------------------------//
#include "DTMuMinusAtomCaptureModel.hh"

#include "celeritas/global/ActionLauncher.hh"
#include "celeritas/global/TrackExecutor.hh"
#include "celeritas/mucf/executor/DTMuMinusAtomCaptureExecutor.hh"  // IWYU pragma: associated
#include "celeritas/phys/InteractionApplier.hh"  // IWYU pragma: associated
#include "celeritas/phys/PDGNumber.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from model ID and other necessary data.
 */
DTMuMinusAtomCaptureModel::DTMuMinusAtomCaptureModel(
    ActionId id,
    ParticleParams const& particles,
    MaterialParams const& materials)
    : ConcreteAction(id,
                     "dt-mu-capture",
                     "interact by muon generating a muonic d or t atom")
{
    CELER_EXPECT(id);

    data_.muon = particles.find(pdg::mu_minus());
    // TODO: better than string?
    data_.deuterium = materials.find_element("deuterium");
    data_.tritium = materials.find_element("tritium");
    CELER_ASSERT(data_);

    CELER_VALIDATE(data_.muon,
                   << "missing negative muon (required for "
                   << this->description() << ")");
    CELER_ENSURE(data_);
}

//---------------------------------------------------------------------------//
/*!
 * Particle types and energy ranges that this model applies to.
 */
auto DTMuMinusAtomCaptureModel::applicability() const -> SetApplicability
{
    Applicability applic;
    applic.particle = data_.muon;
    applic.lower = zero_quantity();  // Valid at rest
    applic.upper = units::MevEnergy{1e8};  // 100 TeV

    return {applic};
}

//---------------------------------------------------------------------------//
/*!
 * Get the microscopic cross sections for the given particle and material.
 */
auto DTMuMinusAtomCaptureModel::micro_xs(Applicability) const -> MicroXsBuilders
{
    // TODO: FIXME
    return {};
}

//---------------------------------------------------------------------------//
/*!
 * Interact with host data.
 */
void DTMuMinusAtomCaptureModel::step(CoreParams const& params,
                                     CoreStateHost& state) const
{
    auto execute = make_action_track_executor(
        params.ptr<MemSpace::native>(),
        state.ptr(),
        this->action_id(),
        InteractionApplier{DTMuMinusAtomCaptureExecutor{this->host_ref()}});
    return launch_action(*this, params, state, execute);
}

//---------------------------------------------------------------------------//
#if !CELER_USE_DEVICE
void DTMuMinusAtomCaptureModel::step(CoreParams const&, CoreStateDevice&) const
{
    CELER_NOT_CONFIGURED("CUDA OR HIP");
}
#endif

//---------------------------------------------------------------------------//
}  // namespace celeritas

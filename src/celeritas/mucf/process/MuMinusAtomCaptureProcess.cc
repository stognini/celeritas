//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/process/MuMinusAtomCaptureProcess.cc
//---------------------------------------------------------------------------//
#include "MuMinusAtomCaptureProcess.hh"

#include <memory>

#include "celeritas/mucf/model/DTMuMinusAtomCaptureModel.hh"
#include "celeritas/phys/Model.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from host data.
 */
MuMinusAtomCaptureProcess::MuMinusAtomCaptureProcess(
    SPConstParticles particles,
    SPConstMaterials materials,
    SPConstImported process_data)
    : particles_(particles)
    , materials_(materials)
    , imported_(process_data,
                particles_,
                ImportProcessClass::muon_atomic_capture,
                {pdg::mu_minus()})
    , muon_id_(particles_->find(pdg::mu_minus()))
{
    CELER_EXPECT(particles_);
    CELER_EXPECT(materials_);
    CELER_EXPECT(muon_id_);
}

//---------------------------------------------------------------------------//
/*!
 * Construct the models associated with this process.
 */
auto MuMinusAtomCaptureProcess::build_models(ActionIdIter start_id) const
    -> VecModel
{
    return {std::make_shared<DTMuMinusAtomCaptureModel>(
        *start_id++, *particles_, *materials_)};
}

//---------------------------------------------------------------------------//
/*!
 * Get the interaction cross sections for the given energy range.
 */
auto MuMinusAtomCaptureProcess::macro_xs(Applicability applic) const -> XsGrid
{
    return {};  // \todo: return from imported_?
}

//---------------------------------------------------------------------------//
/*!
 * Get the energy loss for the given energy range.
 */
auto MuMinusAtomCaptureProcess::energy_loss(Applicability applic) const
    -> EnergyLossGrid
{
    return {};  // \todo: return from imported_?
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

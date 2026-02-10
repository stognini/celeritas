//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/process/MucfProcess.cc
//---------------------------------------------------------------------------//
#include "MucfProcess.hh"

#include <memory>

#include "celeritas/mat/MaterialParams.hh"
#include "celeritas/mucf/model/DTMixMucfModel.hh"
#include "celeritas/phys/Model.hh"
#include "celeritas/phys/ParticleParams.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from host data.
 */
MucfProcess::MucfProcess(SPConstParticles particles, SPConstMaterials materials)
    : particles_(particles), materials_(materials)
{
    CELER_EXPECT(particles_);
    CELER_EXPECT(materials_);
}

//---------------------------------------------------------------------------//
/*!
 * Construct the models associated with this process.
 */
auto MucfProcess::build_models(ActionIdIter start_id) const -> VecModel
{
    auto model = std::make_shared<DTMixMucfModel>(
        *start_id++, *particles_, *materials_);
    model_ = model;
    return {std::move(model)};
}

//---------------------------------------------------------------------------//
/*!
 * Get the interaction cross sections for the given energy range.
 */
auto MucfProcess::macro_xs(Applicability applic) const -> XsGrid
{
    auto const& data = model_->host_ref();
    CELER_ASSERT(data);

    XsGrid result;

    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Get the energy loss for the given energy range.
 */
auto MucfProcess::energy_loss(Applicability) const -> EnergyLossGrid
{
    // No energy loss for at-rest process
    return {};
}
//---------------------------------------------------------------------------//
/*!
 * Name of the process.
 */
std::string_view MucfProcess::label() const
{
    return "Muon-catalyzed fusion";
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

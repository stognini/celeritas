//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/process/MucfProcess.cc
//---------------------------------------------------------------------------//
#include "MucfProcess.hh"

#include <memory>

#include "celeritas/mat/MaterialParams.hh"
#include "celeritas/mucf/Types.hh"
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

    auto find_mucf_matid
        = [](PhysMatId matid, auto const& mucfmatid_to_matid) -> MuCfMatId {
        for (auto id : range(mucfmatid_to_matid.size()))
        {
            MuCfMatId mucf_matid{id};
            if (mucfmatid_to_matid[mucf_matid] == matid)
            {
                return mucf_matid;
            }
        }
        return {};
    };

    auto mucf_matid = find_mucf_matid(applic.material, data.mucfmatid_to_matid);
    if (!mucf_matid)
    {
        // Not a muCF material
        return {};
    }

    // Get total material cycle time
    auto const& mat_cycle_times = data.cycle_times[mucf_matid];
    real_type total_cycle_times{0};
    for (auto mol : range(MucfMuonicMolecule::size_))
    {
        for (auto spin : range(2))
        {
            total_cycle_times += mat_cycle_times[mol][spin];
        }
    }

    real_type cycle_rate = real_type{1} / total_cycle_times;  // [1/s]

    // Constant "XS" grid with constant cycle rates
    XsGrid result;
    result.lower.x[Bound::lo] = applic.lower.value();
    result.upper.x[Bound::hi] = applic.upper.value();
    result.lower.y = {cycle_rate, cycle_rate};
    result.upper.y = {cycle_rate, cycle_rate};
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

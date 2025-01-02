//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/model/MuPairProductionModel.cc
//---------------------------------------------------------------------------//
#include "MuPairProductionModel.hh"

#include <utility>
#include <vector>

#include "corecel/Config.hh"

#include "corecel/cont/Range.hh"
#include "corecel/data/Collection.hh"
#include "corecel/data/CollectionBuilder.hh"
#include "corecel/data/HyperslabIndexer.hh"
#include "corecel/sys/ScopedMem.hh"
#include "celeritas/em/executor/MuPairProductionExecutor.hh"
#include "celeritas/em/interactor/detail/PhysicsConstants.hh"
#include "celeritas/global/ActionLauncher.hh"
#include "celeritas/global/CoreParams.hh"
#include "celeritas/global/TrackExecutor.hh"
#include "celeritas/grid/TwodGridBuilder.hh"
#include "celeritas/io/ImportProcess.hh"
#include "celeritas/phys/InteractionApplier.hh"
#include "celeritas/phys/PDGNumber.hh"
#include "celeritas/phys/ParticleParams.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from model ID and shared data.
 */
MuPairProductionModel::MuPairProductionModel(
    ActionId id,
    ParticleParams const& particles,
    SPConstImported data,
    ImportMuPairProductionTable const& imported)
    : StaticConcreteAction(
        id, "pair-prod-muon", "interact by e-/e+ pair production by muons")
    , imported_(data,
                particles,
                ImportProcessClass::mu_pair_prod,
                ImportModelClass::mu_pair_prod,
                {pdg::mu_minus(), pdg::mu_plus()})
{
    CELER_EXPECT(id);

    ScopedMem record_mem("MuPairProductionModel.construct");

    HostVal<MuPairProductionData> host_data;

    // Save IDs
    host_data.ids.mu_minus = particles.find(pdg::mu_minus());
    host_data.ids.mu_plus = particles.find(pdg::mu_plus());
    host_data.ids.electron = particles.find(pdg::electron());
    host_data.ids.positron = particles.find(pdg::positron());
    CELER_VALIDATE(host_data.ids,
                   << "missing particles (required for '"
                   << this->description() << "')");

    // Save particle properties
    host_data.electron_mass = particles.get(host_data.ids.electron).mass();

    // Build sampling table
    CELER_VALIDATE(imported,
                   << "sampling table (required for '" << this->description()
                   << "') is empty");
    this->build_table(imported, &host_data.table);

    // Move to mirrored data, copying to device
    data_ = CollectionMirror<MuPairProductionData>{std::move(host_data)};

    CELER_ENSURE(data_);
}

//---------------------------------------------------------------------------//
/*!
 * Particle types and energy ranges that this model applies to.
 */
auto MuPairProductionModel::applicability() const -> SetApplicability
{
    Applicability mu_minus_applic;
    mu_minus_applic.particle = this->host_ref().ids.mu_minus;
    mu_minus_applic.lower = zero_quantity();
    mu_minus_applic.upper = detail::high_energy_limit();

    Applicability mu_plus_applic = mu_minus_applic;
    mu_plus_applic.particle = this->host_ref().ids.mu_plus;

    return {mu_minus_applic, mu_plus_applic};
}

//---------------------------------------------------------------------------//
/*!
 * Get the microscopic cross sections for the given particle and material.
 */
auto MuPairProductionModel::micro_xs(Applicability applic) const
    -> MicroXsBuilders
{
    return imported_.micro_xs(std::move(applic));
}

//---------------------------------------------------------------------------//
/*!
 * Interact with host data.
 */
void MuPairProductionModel::step(CoreParams const& params,
                                 CoreStateHost& state) const
{
    auto execute = make_action_track_executor(
        params.ptr<MemSpace::native>(),
        state.ptr(),
        this->action_id(),
        InteractionApplier{MuPairProductionExecutor{this->host_ref()}});
    return launch_action(*this, params, state, execute);
}

//---------------------------------------------------------------------------//
#if !CELER_USE_DEVICE
void MuPairProductionModel::step(CoreParams const&, CoreStateDevice&) const
{
    CELER_NOT_CONFIGURED("CUDA OR HIP");
}
#endif

//---------------------------------------------------------------------------//
/*!
 * Construct sampling table.
 */
void MuPairProductionModel::build_table(
    ImportMuPairProductionTable const& imported, HostTable* table) const
{
    CELER_EXPECT(imported);
    CELER_EXPECT(table);

    // Build 2D sampling table
    TwodGridBuilder build_grid{&table->reals};
    CollectionBuilder grids{&table->grids};
    for (auto const& pvec : imported.physics_vectors)
    {
        CELER_VALIDATE(pvec,
                       << "invalid grid in sampling table for '"
                       << this->description() << "'");

        Array<size_type, 2> dims{static_cast<size_type>(pvec.x.size()),
                                 static_cast<size_type>(pvec.y.size())};
        HyperslabIndexer index(dims);

        // Normalize the CDF
        std::vector<double> cdf(pvec.value.size());
        for (size_type i : range(dims[0]))
        {
            double norm = 1 / pvec.value[index(i, dims[1] - 1)];
            for (size_type j : range(dims[1]))
            {
                cdf[index(i, j)] = pvec.value[index(i, j)] * norm;
            }
        }
        grids.push_back(
            build_grid(make_span(pvec.x), make_span(pvec.y), make_span(cdf)));
    }

    // Build log Z grid
    std::vector<real_type> log_z;
    log_z.reserve(imported.atomic_number.size());
    for (auto z : imported.atomic_number)
    {
        log_z.push_back(std::log(z));
    }
    table->logz_grid
        = make_builder(&table->reals).insert_back(log_z.begin(), log_z.end());

    CELER_ENSURE(*table);
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

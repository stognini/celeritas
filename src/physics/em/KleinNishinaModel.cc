//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file KleinNishinaModel.cc
//---------------------------------------------------------------------------//
#include "KleinNishinaModel.hh"

#include "base/Assert.hh"
#include "physics/base/PDGNumber.hh"
#include "physics/em/generated/KleinNishinaInteract.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from model ID and other necessary data.
 */
KleinNishinaModel::KleinNishinaModel(ModelId               id,
                                     const ParticleParams& particles)
{
    CELER_EXPECT(id);
    interface_.ids.model    = id;
    interface_.ids.electron = particles.find(pdg::electron());
    interface_.ids.gamma    = particles.find(pdg::gamma());

    CELER_VALIDATE(interface_.ids.electron && interface_.ids.gamma,
                   << "missing electron, positron and/or gamma particles "
                      "(required for "
                   << this->label() << ")");
    interface_.inv_electron_mass
        = 1 / particles.get(interface_.ids.electron).mass().value();
    CELER_ENSURE(interface_);
}

//---------------------------------------------------------------------------//
/*!
 * Particle types and energy ranges that this model applies to.
 */
auto KleinNishinaModel::applicability() const -> SetApplicability
{
    Applicability photon_applic;
    photon_applic.particle = interface_.ids.gamma;
    photon_applic.lower    = zero_quantity();
    photon_applic.upper    = max_quantity();

    return {photon_applic};
}

//---------------------------------------------------------------------------//
//!@{
/*!
 * Apply the interaction kernel.
 */
void KleinNishinaModel::interact(const DeviceInteractRef& data) const
{
    generated::klein_nishina_interact(interface_, data);
}

void KleinNishinaModel::interact(const HostInteractRef& data) const
{
    generated::klein_nishina_interact(interface_, data);
}

//---------------------------------------------------------------------------//
/*!
 * Get the model ID for this model.
 */
ModelId KleinNishinaModel::model_id() const
{
    return interface_.ids.model;
}

//!@}
//---------------------------------------------------------------------------//
} // namespace celeritas

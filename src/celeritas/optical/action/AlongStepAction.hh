//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/optical/action/AlongStepAction.hh
//---------------------------------------------------------------------------//
#pragma once

#include "ActionInterface.hh"

namespace celeritas
{
namespace optical
{
//---------------------------------------------------------------------------//
/*!
 * Move a track to the next boundary or interaction.
 */
class AlongStepAction final : public OpticalStepActionInterface,
                              public StaticConcreteAction
{
  public:
    // Construct with ID
    explicit AlongStepAction(ActionId);

    // Launch kernel with host data
    void step(CoreParams const&, CoreStateHost&) const final;

    // Launch kernel with device data
    void step(CoreParams const&, CoreStateDevice&) const final;

    //! Dependency ordering of the action
    StepActionOrder order() const final { return StepActionOrder::along; }
};

//---------------------------------------------------------------------------//
}  // namespace optical
}  // namespace celeritas

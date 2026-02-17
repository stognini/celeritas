//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/mucf/model/AtRestModel.hh
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas/phys/Model.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Abstract model class for at-rest models.
 *
 * Adds the capability of storing the total interaction rate for each model
 * as opposed to cross section.
 */
class AtRestModel : public Model
{
  public:
    //! Get the applicable interaction rate for this at-rest model
    virtual real_type interaction_rate(Applicability) const = 0;
};

//---------------------------------------------------------------------------//
}  // namespace celeritas

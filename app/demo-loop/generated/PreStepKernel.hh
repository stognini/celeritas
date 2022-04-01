//----------------------------------*-C++-*----------------------------------//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file demo-loop/generated/PreStepKernel.hh
//! \note Auto-generated by gen-demo-loop-kernel.py: DO NOT MODIFY!
//---------------------------------------------------------------------------//
#include "base/Assert.hh"
#include "base/Macros.hh"
#include "sim/CoreTrackData.hh"

namespace demo_loop
{
namespace generated
{
void pre_step(celeritas::CoreHostRef const&);

void pre_step(celeritas::CoreDeviceRef const&);

#if !CELER_USE_DEVICE
inline void pre_step(celeritas::CoreDeviceRef const&)
{
    CELER_NOT_CONFIGURED("CUDA OR HIP");
}
#endif

} // namespace generated
} // namespace demo_loop

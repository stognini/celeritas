//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file physics/em/generated/SeltzerBergerInteract.cu
//! \note Auto-generated by gen-interactor.py: DO NOT MODIFY!
//---------------------------------------------------------------------------//
#include "base/device_runtime_api.h"
#include "base/Assert.hh"
#include "base/KernelParamCalculator.device.hh"
#include "comm/Device.hh"
#include "../detail/SeltzerBergerLauncher.hh"

using namespace celeritas::detail;

namespace celeritas
{
namespace generated
{
namespace
{
__global__ void seltzer_berger_interact_kernel(
    const detail::SeltzerBergerDeviceRef seltzer_berger_data,
    const ModelInteractRef<MemSpace::device> model)
{
    auto tid = KernelParamCalculator::thread_id();
    if (!(tid < model.states.size()))
        return;

    detail::SeltzerBergerLauncher<MemSpace::device> launch(seltzer_berger_data, model);
    launch(tid);
}
} // namespace

void seltzer_berger_interact(
    const detail::SeltzerBergerDeviceRef& seltzer_berger_data,
    const ModelInteractRef<MemSpace::device>& model)
{
    CELER_EXPECT(seltzer_berger_data);
    CELER_EXPECT(model);
    CELER_LAUNCH_KERNEL(seltzer_berger_interact,
                        celeritas::device().default_block_size(),
                        model.states.size(),
                        seltzer_berger_data, model);
}

} // namespace generated
} // namespace celeritas

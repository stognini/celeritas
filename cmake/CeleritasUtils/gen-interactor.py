#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
"""
Tool to generate Model "interact" implementations on the fly.

Assumptions:
 - The class's header defines ClassDeviceRef and ClassDeviceHost type aliases
   for the host/device collection groups.
"""

import os.path
import sys
from launchbounds import make_launch_bounds

CLIKE_TOP = '''\
//{modeline:-^75s}//
// Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \\file {filename}
//! \\note Auto-generated by {script}: DO NOT MODIFY!
//---------------------------------------------------------------------------//
'''

HH_TEMPLATE = CLIKE_TOP + """\
#include "base/Assert.hh"
#include "base/Macros.hh"
#include "../detail/{class}Data.hh"

namespace celeritas
{{
namespace generated
{{
void {func}_interact(
    const celeritas::detail::{class}HostRef&,
    const CoreRef<MemSpace::host>&);

void {func}_interact(
    const celeritas::detail::{class}DeviceRef&,
    const CoreRef<MemSpace::device>&);

#if !CELER_USE_DEVICE
inline void {func}_interact(
    const celeritas::detail::{class}DeviceRef&,
    const CoreRef<MemSpace::device>&)
{{
    CELER_ASSERT_UNREACHABLE();
}}
#endif

}} // namespace generated
}} // namespace celeritas
"""

CC_TEMPLATE = CLIKE_TOP + """\
#include "../detail/{class}Launcher.hh"

#include "base/Assert.hh"
#include "base/Types.hh"
#include "physics/base/InteractionLauncher.hh"

namespace celeritas
{{
namespace generated
{{
void {func}_interact(
    const celeritas::detail::{class}HostRef& model_data,
    const CoreRef<MemSpace::host>& core_data)
{{
    CELER_EXPECT(core_data);
    CELER_EXPECT(model_data);

    auto launch = make_interaction_launcher(
        core_data,
        model_data,
        celeritas::detail::{func}_interact_track);
    #pragma omp parallel for
    for (size_type i = 0; i < core_data.states.size(); ++i)
    {{
        ThreadId tid{{i}};
        launch(tid);
    }}
}}

}} // namespace generated
}} // namespace celeritas
"""

CU_TEMPLATE = CLIKE_TOP + """\
#include "base/device_runtime_api.h"

#include "base/Assert.hh"
#include "base/Types.hh"
#include "base/KernelParamCalculator.device.hh"
#include "comm/Device.hh"
#include "physics/base/InteractionLauncher.hh"

#include "../detail/{class}Launcher.hh"

namespace celeritas
{{
namespace generated
{{
namespace
{{
__global__ void{launch_bounds}{func}_interact_kernel(
    const celeritas::detail::{class}DeviceRef model_data,
    const CoreRef<MemSpace::device> core_data)
{{
    auto tid = KernelParamCalculator::thread_id();
    if (!(tid < core_data.states.size()))
        return;

    auto launch = make_interaction_launcher(
        core_data,
        model_data,
        celeritas::detail::{func}_interact_track);
    launch(tid);
}}
}} // namespace

void {func}_interact(
    const celeritas::detail::{class}DeviceRef& model_data,
    const CoreRef<MemSpace::device>& core_data)
{{
    CELER_EXPECT(core_data);
    CELER_EXPECT(model_data);

    CELER_LAUNCH_KERNEL({func}_interact,
                        celeritas::device().default_block_size(),
                        core_data.states.size(),
                        model_data, core_data);
}}

}} // namespace generated
}} // namespace celeritas
"""

TEMPLATES = {
    'hh': HH_TEMPLATE,
    'cc': CC_TEMPLATE,
    'cu': CU_TEMPLATE,
}
LANG = {
    'hh': "C++",
    'cc': "C++",
    'cu': "CUDA",
}

def generate(**subs):
    ext = subs['ext']
    subs['modeline'] = "-*-{}-*-".format(LANG[ext])
    template = TEMPLATES[ext]
    filename = "{basename}.{ext}".format(**subs)
    subs['filename'] = filename
    subs['script'] = os.path.basename(sys.argv[0])
    subs['launch_bounds'] = make_launch_bounds(subs['func'] + '_interact')
    with open(filename, 'w') as f:
        f.write(template.format(**subs))

def main():
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--basename',
        help='File name (without extension) of output')
    parser.add_argument(
        '--class',
        help='CamelCase name of the class prefix')
    parser.add_argument(
        '--func',
        help='snake_case name of the interact function prefix')

    kwargs = vars(parser.parse_args())
    for ext in ['hh', 'cc', 'cu']:
        generate(ext=ext, **kwargs)

if __name__ == '__main__':
    main()

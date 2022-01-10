#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2021-2022 UT-Battelle, LLC, and other Celeritas developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
"""
Tool to generate demo loop kernel implementations on the fly.

Assumptions:
 - Generated in a subdirectory (preferably named ``generated``) below the
   directory containing ``LDemoLauncher.hh''
"""

FILENAME = "{class}Kernel.{ext}"

CLIKE_TOP = '''\
//{modeline:-^75s}//
// Copyright {year} UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \\file {filename}
//! \\note Auto-generated by gen-demo-loop-kernel.py: DO NOT MODIFY!
//---------------------------------------------------------------------------//
'''

HH_TEMPLATE = CLIKE_TOP + """\
#include "celeritas_config.h"
#include "base/Assert.hh"

namespace demo_loop
{{
namespace generated
{{
void {func}(
    const celeritas::ParamsHostRef&,
    const celeritas::StateHostRef&);

void {func}(
    const celeritas::ParamsDeviceRef&,
    const celeritas::StateDeviceRef&);

#if !CELERITAS_USE_CUDA
inline void {func}(
    const celeritas::ParamsDeviceRef&,
    const celeritas::StateDeviceRef&)
{{
    CELER_NOT_CONFIGURED("CUDA");
}}
#endif

}} // namespace generated
}} // namespace demo_loop
"""

CC_TEMPLATE = CLIKE_TOP + """\
#include "base/Assert.hh"
#include "base/Types.hh"
#include "../LDemoLauncher.hh"

using namespace celeritas;

namespace demo_loop
{{
namespace generated
{{
void {func}(
    const ParamsHostRef& params,
    const StateHostRef& states)
{{
    CELER_EXPECT(params);
    CELER_EXPECT(states);

    {class}Launcher<MemSpace::host> launch(params, states);
    #pragma omp parallel for
    for (size_type i = 0; i < {threads}; ++i)
    {{
        launch(ThreadId{{i}});
    }}
}}

}} // namespace generated
}} // namespace demo_loop
"""

CU_TEMPLATE = CLIKE_TOP + """\
#include "base/Assert.hh"
#include "base/Types.hh"
#include "base/KernelParamCalculator.cuda.hh"
#include "../LDemoLauncher.hh"

using namespace celeritas;

namespace demo_loop
{{
namespace generated
{{
namespace
{{
__global__ void {func}_kernel(
    ParamsDeviceRef const params,
    StateDeviceRef const states)
{{
    auto tid = KernelParamCalculator::thread_id();
    if (!(tid < {threads}))
        return;

    {class}Launcher<MemSpace::device> launch(params, states);
    launch(tid);
}}
}} // namespace

void {func}(
    const celeritas::ParamsDeviceRef& params,
    const celeritas::StateDeviceRef& states)
{{
    CELER_EXPECT(params);
    CELER_EXPECT(states);

    static const KernelParamCalculator {func}_ckp(
        {func}_kernel, "{func}");
    auto kp = {func}_ckp({threads});
    {func}_kernel<<<kp.grid_size, kp.block_size>>>(
        params, states);
    CELER_CUDA_CHECK_ERROR();
}}

}} // namespace generated
}} // namespace demo_loop
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
    subs['modeline'] = "-*-{}-*-".format(ext)
    template = TEMPLATES[ext]
    filename = FILENAME.format(**subs)
    subs['filename'] = filename
    with open(filename, 'w') as f:
        f.write(template.format(**subs))

def main():
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--class',
        help='CamelCase name of the class prefix')
    parser.add_argument(
        '--func',
        help='snake_case name of the function')
    parser.add_argument(
        '--threads',
        default='states.size()',
        help='String describing the number of threads')

    kwargs = vars(parser.parse_args())
    kwargs['year'] = 2021
    for ext in ['hh', 'cc', 'cu']:
        generate(ext=ext, **kwargs)

if __name__ == '__main__':
    main()
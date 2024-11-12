//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geocel/GeoVolumeFinder.cc
//---------------------------------------------------------------------------//
#include "GeoVolumeFinder.hh"

#include "corecel/io/Join.hh"
#include "corecel/io/Logger.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Perform the search.
 */
VolumeId GeoVolumeFinder::operator()(Label const& label) const noexcept
{
    if (auto id = vols_.find_exact(label))
    {
        // Exact match
        return id;
    }

    // Fall back to skipping the extension: look for all possible matches
    auto all_ids = vols_.find_all(label.name);
    if (all_ids.size() == 1)
    {
        if (!label.ext.empty())
        {
            CELER_LOG(warning)
                << "Failed to exactly match " << celeritas_core_geo
                << " volume from volume '" << label << "'; found '"
                << vols_.at(all_ids.front()) << "' by omitting the extension";
        }
        return all_ids.front();
    }
    if (all_ids.size() > 1)
    {
        CELER_LOG(warning) << "Multiple volumes '"
                           << join(all_ids.begin(),
                                   all_ids.end(),
                                   "', '",
                                   [this](VolumeId v) { return vols_.at(v); })
                           << "' match the name '" << label.name
                           << "': returning the last one";
        return all_ids.back();
    }
    return {};
}

//---------------------------------------------------------------------------//
}  // namespace celeritas

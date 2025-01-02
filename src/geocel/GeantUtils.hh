//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geocel/GeantUtils.hh
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Config.hh"

#include "corecel/Assert.hh"

class G4RunManager;

namespace celeritas
{
//---------------------------------------------------------------------------//
// Clear Geant4's signal handlers that get installed when linking 11+
void disable_geant_signal_handler();

//---------------------------------------------------------------------------//
// Get the number of threads in a version-portable way
int get_geant_num_threads(G4RunManager const&);

//---------------------------------------------------------------------------//
// Get the number of threads using the global G4 run manager
int get_geant_num_threads();

//---------------------------------------------------------------------------//
// Get the current thread ID (zero if serial)
int get_geant_thread_id();

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
#if !CELERITAS_USE_GEANT4
inline void disable_geant_signal_handler() {}

inline int get_geant_num_threads(G4RunManager const&)
{
    CELER_NOT_CONFIGURED("Geant4");
}

inline int get_geant_num_threads()
{
    CELER_NOT_CONFIGURED("Geant4");
}

inline int get_geant_thread_id()
{
    CELER_NOT_CONFIGURED("Geant4");
}
#endif
//---------------------------------------------------------------------------//
}  // namespace celeritas

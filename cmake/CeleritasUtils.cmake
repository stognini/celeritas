#------------------------------- -*- cmake -*- -------------------------------#
# Copyright Celeritas contributors: see top-level COPYRIGHT file for details
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
#[=======================================================================[.rst:

CeleritasUtils
--------------

Miscellaneous utility functions.

.. command:: celeritas_version_to_hex

  Convert a version number to a C-formatted hexadecimal string::

    celeritas_version_to_hex(<var> <version_prefix>)

  The code will set a version to zero if ``<version_prefix>_VERSION`` is not
  found. If ``<version_prefix>_MAJOR`` is defined it will use that; otherwise
  it will try to split the version into major/minor/patch.

.. command:: celeritas_get_g4libs

  Construct a list of Geant4 libraries for linking by adding a ``Geant4::G4``
  prefix and the necessary suffix::

    celeritas_get_g4libs(<var> [<lib> ...])

  Example::

    celeritas_get_g4libs(_g4libs geometry persistency)
    target_link_library(foo ${_g4libs})

  If Geant4 is unavailable, the result will be empty.

.. command:: celeritas_set_celer_g4env

  Set the ``CELER_G4ENV`` cache variable from the Geant4 library CMake's
  configured data location, overriding from environment variables if present.
  This variable is persistent across configurations.

#]=======================================================================]
include_guard(GLOBAL)

#-----------------------------------------------------------------------------#

function(celeritas_version_to_hex var version)
  if(NOT DEFINED "${version}_MAJOR" AND DEFINED "${version}")
    # Split version into components
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" _match "${${version}}")
    if(NOT _match)
      message(AUTHOR_WARNING
        "Failed to parse version string for ${version}=\"${${version}}\":
        _match=${_match}"
      )
    endif()
    set(${version}_MAJOR "${CMAKE_MATCH_1}")
    set(${version}_MINOR "${CMAKE_MATCH_2}")
    set(${version}_PATCH "${CMAKE_MATCH_3}")
  endif()
  # Set any empty or undefined values to zero
  foreach(_ext MAJOR MINOR PATCH)
    if(NOT ${version}_${_ext})
      set(${version}_${_ext} 0)
    endif()
  endforeach()
  # Add an extra 1 up front and strip it to zero-pad
  math(EXPR _temp_version
    "((256 + ${${version}_MAJOR}) * 256 + ${${version}_MINOR}) * 256 + ${${version}_PATCH}"
    OUTPUT_FORMAT HEXADECIMAL
  )
  string(SUBSTRING "${_temp_version}" 3 -1 _temp_version)
  set(${var} "0x${_temp_version}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------------#

function(celeritas_get_g4libs var)
  set(_result)
  if(NOT CELERITAS_USE_Geant4)
    # No Geant4, no libs
  elseif(Geant4_VERSION VERSION_LESS 10.6)
    # Old Geant4 doesn't have granularity
    set(_result ${Geant4_LIBRARIES})
  else()
    # Determine the default library extension
    if(TARGET Geant4::G4global-static)
      set(_g4_static TRUE)
    else()
      set(_g4_static FALSE)
    endif()
    if(TARGET Geant4::G4global)
      set(_g4_shared TRUE)
    else()
      set(_g4_shared FALSE)
    endif()
    if(NOT _g4_static OR (BUILD_SHARED_LIBS AND _g4_shared))
      # Use shared if static is unavailable, or if shared is available and we're
      # building shared
      set(_ext "")
    else()
      set(_ext "-static")
    endif()

    foreach(_shortlib IN LISTS ARGN)
      set(_lib Geant4::G4${_shortlib}${_ext})
      if(TARGET "${_lib}")
        list(APPEND _result "${_lib}")
      elseif(_shortlib STREQUAL "persistency")
        # Workaround for differing target names in 11.2.0,1 for G4persistency
        # We do this here and not in FindGeant4 because we have no
        # guarantee projects using Celeritas and Geant4 won't mess with target
        # names themselves (and if we had to create a "celeritas::" target,
        # we'd still have to specialize here).
        list(APPEND _result Geant4::G4mctruth${_ext} Geant4::G4geomtext${_ext})
        if(TARGET "Geant4::G4gdml${_ext}")
          list(APPEND _result Geant4::G4gdml${_ext})
        endif()
      elseif(_shortlib STREQUAL "tasking")
        # Same workaround for tasking, which was split between G4global and G4run
        # from 11.1
        list(APPEND _result Geant4::G4run${_ext} Geant4::G4global${_ext})
      else()
        message(AUTHOR_WARNING "No Geant4 library '${_lib}' exists")
      endif()
    endforeach()
    # This avoids "ld: warning: ignoring duplicate libraries:"
    list(REMOVE_DUPLICATES _result)
  endif()
  set(${var} "${_result}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------------#

function(celeritas_set_celer_g4env)
  set(_CELER_G4ENV)
  foreach(_ds IN LISTS Geant4_DATASETS)
    set(_key ${Geant4_DATASET_${_ds}_ENVVAR})
    set(_val ${Geant4_DATASET_${_ds}_PATH})
    set(_env "$ENV{${_key}}")
    if(_env AND NOT _env STREQUAL _val)
      message(VERBOSE "CELER_G4ENV: using ${_key}=${_env} from user environment")
      list(APPEND _CELER_G4ENV "${_key}=${_env}")
    else()
      list(APPEND _CELER_G4ENV "${_key}=${_val}")
    endif()
  endforeach()
  set(CELER_G4ENV "${_CELER_G4ENV}" CACHE INTERNAL "Environment variables used for CTest")
endfunction()

#-----------------------------------------------------------------------------#

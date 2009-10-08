# - Try to find Polkit-qt-1
# Once done this will define
#
#  POLKITQT1_FOUND - system has Polkit-qt-1
#  POLKITQT1_INCLUDE_DIR - the Polkit-qt-1 include directory
#  POLKITQT1_LIBRARIES - Link these to use all Polkit-qt-1 libs
#  POLKITQT1_CORE_LIBRARY
#  POLKITQT1_GUI_LIBRARY
#  POLKITQT1_DEFINITIONS - Compiler switches required for using Polkit-qt-1

# Copyright (c) 2008, Adrien Bustany, <madcat@mymadcat.com>
# Copyright (c) 2009, Daniel Nicoletti, <dantti85-pk@yahoo.com.br>
# Copyright (c) 2009, Dario Freddi, <drf54321@gmail.com>
# Copyright (c) 2009, Michal Malek, <michalm@jabster.pl>
# Copyright (c) 2009, Jaroslav Reznik, <jreznik@redhat.com>
#
# Redistribution and use is allowed according to the terms of the GPLv2+ license.

if (POLKITQT1_INCLUDE_DIR AND POLKITQT1_LIB)
    set(POLKITQT1_FIND_QUIETLY TRUE)
endif (POLKITQT1_INCLUDE_DIR AND POLKITQT1_LIB)

include(FindPkgConfig)

if (NOT POLKITQT1_MIN_VERSION)
  set(POLKITQT1_MIN_VERSION "0.95.0")
endif (NOT POLKITQT1_MIN_VERSION)

pkg_search_module( POLKITQT1 polkit-qt-1 )

if (POLKITQT1_FOUND)
    if (POLKITQT1_VERSION VERSION_LESS POLKITQT1_MIN_VERSION)
        message(STATUS "Found Polkit-Qt-1 release < ${POLKITQT1_MIN_VERSION}")
        message(STATUS "You need Polkit-Qt-1 version ${POLKITQT1_MIN_VERSION} or newer to compile this component")
        set(POLKITQT1_FOUND FALSE)
        return()
    else (POLKITQT1_VERSION VERSION_LESS POLKITQT1_MIN_VERSION)
        message(STATUS "Found Polkit-Qt-1 release >= ${POLKITQT1_MIN_VERSION}")
    endif (POLKITQT1_VERSION VERSION_LESS POLKITQT1_MIN_VERSION)
else (POLKITQT1_FOUND)
    set(POLKITQT1_FOUND FALSE)
    message(STATUS "Cannot find Polkit-Qt-1 library!")
    return()
endif (POLKITQT1_FOUND)


find_path( POLKITQT1_INCLUDE_DIR polkit-qt-1/ )

find_library( POLKITQT1_CORE_LIBRARY NAMES polkit-qt-core-1 )
find_library( POLKITQT1_GUI_LIBRARY NAMES polkit-qt-gui-1 )
find_library( POLKITQT1_GUI_LIBRARY NAMES polkit-qt-agent-1 )


if (POLKITQT1_INCLUDE_DIR AND POLKITQT1_CORE_LIBRARY AND POLKITQT1_GUI_LIBRARY)
   set(POLKITQT1_FOUND TRUE)
else (POLKITQT1_INCLUDE_DIR AND POLKITQT1_CORE_LIBRARY AND POLKITQT1_GUI_LIBRARY)
   set(POLKITQT1_FOUND FALSE)
endif (POLKITQT1_INCLUDE_DIR AND POLKITQT1_CORE_LIBRARY AND POLKITQT1_GUI_LIBRARY)

set(POLKITQT1_LIBRARIES ${POLKITQT1_CORE_LIBRARY} ${POLKITQT1_GUI_LIBRARY} ${POLKITQT1_AGENT_LIBRARY})

set(POLKITQT1_INCLUDE_DIR ${POLKITQT1_INCLUDE_DIR}/polkit-qt-1)

set(POLICY_FILES_INSTALL_DIR ${POLKITQT1_PREFIX}/share/polkit-1/actions/)

if (POLKITQT1_FOUND)
  if (NOT POLKITQT1_FIND_QUIETLY)
    message(STATUS "Found Polkit-Qt-1: ${POLKITQT1_LIBRARIES}")
  endif (NOT POLKITQT1_FIND_QUIETLY)
else (POLKITQT1_FOUND)
  if (POLKITQT1_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Polkit-Qt-1")
  endif (POLKITQT1_FIND_REQUIRED)
endif (POLKITQT1_FOUND)

mark_as_advanced(POLKITQT1_INCLUDE_DIR POLKITQT1_LIB)

macro(dbus_add_activation_system_service _sources)
    pkg_search_module( DBUS dbus-1 )
    foreach (_i ${_sources})
        get_filename_component(_service_file ${_i} ABSOLUTE)
        string(REGEX REPLACE "\\.service.*$" ".service" _output_file ${_i})
        set(_target ${CMAKE_CURRENT_BINARY_DIR}/${_output_file})
        configure_file(${_service_file} ${_target})
        install(FILES ${_target} DESTINATION ${DBUS_PREFIX}/share/dbus-1/system-services )
    endforeach (_i ${ARGN})
endmacro(dbus_add_activation_system_service _sources)

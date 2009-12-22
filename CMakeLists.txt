## Polkit Qt Library
project("Polkit-qt-1")

cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(QT_MIN_VERSION "4.4.0")

find_package(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)

include (${QT_USE_FILE})
include (InstallSettings)
include (FindPkgConfig)

pkg_check_modules (POLKIT-1 REQUIRED polkit-gobject-1>=0.95)
pkg_check_modules (POLKIT-1_AGENT REQUIRED polkit-agent-1>=0.95)

message(STATUS " ${POLKIT-1_INCLUDE_DIRS} ===========")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${POLKIT-1_INCLUDE_DIRS}
    ${POLKIT-1_AGENT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${QT_QTXML_INCLUDE_DIR}
)

set(CMAKE_MAJOR_POLKITQT-1_VERSION 0 CACHE INT "PolkitQt-1's major version number" FORCE)
set(CMAKE_MINOR_POLKITQT-1_VERSION 95 CACHE INT "PolkitQt-1's minor version number" FORCE)
set(CMAKE_RELEASE_POLKITQT-1_VERSION 1 CACHE INT "PolkitQt-1's release version number" FORCE)
set(CMAKE_POLKITQT-1_VERSION_STRING "${CMAKE_MAJOR_POLKITQT-1_VERSION}.${CMAKE_MINOR_POLKITQT-1_VERSION}.${CMAKE_RELEASE_POLKITQT-1_VERSION}")

configure_file(polkitqt1version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkitqt1version.h)

install(FILES
    gui/action.h
    gui/actionbutton.h
    gui/actionbuttons.h

    core/authority.h
    core/details.h
    core/identity.h
    core/subject.h
    core/temporaryauthorization.h
    core/actiondescription.h

    agent/listener.h
    agent/session.h

    ${CMAKE_CURRENT_BINARY_DIR}/polkitqt1version.h
    export.h

    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1 COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Action
    includes/PolkitQt1/Authority
    includes/PolkitQt1/ActionButton
    includes/PolkitQt1/ActionButtons
    includes/PolkitQt1/Details
    includes/PolkitQt1/Identity
    includes/PolkitQt1/Subject
    includes/PolkitQt1/TemporaryAuthorization
    includes/PolkitQt1/ActionDescription
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt1 COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Agent/Listener
    includes/PolkitQt1/Agent/Session
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt1/Agent COMPONENT Devel)


if(NOT WIN32)
# Pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-core-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-gui-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-agent-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
endif(NOT WIN32)

option(BUILD_EXAMPLES "Builds a set of examples for polkit-qt-1" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(cmake)

option(BUILD_TEST "Builds unit tests for polkit-qt-1" OFF)
if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(agent)


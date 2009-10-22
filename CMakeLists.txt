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

pkg_check_modules (POLKIT REQUIRED polkit-gobject-1>=0.95)
pkg_check_modules (POLKIT_AGENT REQUIRED polkit-agent-1>=0.95)

message(STATUS " ${POLKIT_INCLUDE_DIRS} ===========")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${POLKIT_INCLUDE_DIRS}
    ${POLKITDBUS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${QT_QTXML_INCLUDE_DIR}
)

set(MAJOR_POLKIT_QT_VERSION "0")
set(MINOR_POLKIT_QT_VERSION "95")
set(PATCH_POLKIT_QT_VERSION "0")
set(POLKIT_QT_VERSION_STRING "${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}")

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
    export.h

    includes/PolkitQt/Action
    includes/PolkitQt/Authority
    includes/PolkitQt/ActionButton
    includes/PolkitQt/ActionButtons
    includes/PolkitQt/Details
    includes/PolkitQt/Identity
    includes/PolkitQt/Subject
    includes/PolkitQt/TemporaryAuthorization
    includes/PolkitQt/ActionDescription
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt COMPONENT Devel)

install(FILES
    agent/listener.h
    agent/session.h

    export.h

    includes/PolkitQtAgent/Listener
    includes/PolkitQtAgent/Session
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQtAgent COMPONENT Devel)


# Pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-core-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-gui-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-agent-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent-1.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )

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


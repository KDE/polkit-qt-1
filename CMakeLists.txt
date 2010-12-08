## Polkit Qt Library
project("PolkitQt-1")

cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(QT_MIN_VERSION "4.4.0")

find_package(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(Polkit REQUIRED)
find_package(GObject REQUIRED)
find_package(GIO REQUIRED)

add_definitions(-DQT_NO_KEYWORDS)

include (${QT_USE_FILE})
include (InstallSettings)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${POLKIT_INCLUDE_DIR}
    ${POLKIT_AGENT_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
    ${GOBJECT_INCLUDE_DIR}
    ${GIO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${QT_QTXML_INCLUDE_DIR}
)

set(POLKITQT-1_VERSION_MAJOR 0 CACHE INT "PolkitQt-1's major version number" FORCE)
set(POLKITQT-1_VERSION_MINOR 98 CACHE INT "PolkitQt-1's minor version number" FORCE)
set(POLKITQT-1_VERSION_PATCH 1 CACHE INT "PolkitQt-1's release version number" FORCE)
set(POLKITQT-1_VERSION_STRING
    "${POLKITQT-1_VERSION_MAJOR}.${POLKITQT-1_VERSION_MINOR}.${POLKITQT-1_VERSION_PATCH}")

# The SONUMBER of the polkit-qt-1 libraries
set(POLKITQT-1_ABI_VERSION 1)
# The version number of the polkit-qt-1 libraries
set(POLKITQT-1_LIBRARY_VERSION "${POLKITQT-1_ABI_VERSION}.${POLKITQT-1_VERSION_MINOR}.${POLKITQT-1_VERSION_PATCH}")

configure_file(polkitqt1-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkitqt1-version.h)

install(FILES
    gui/polkitqt1-gui-action.h
    gui/polkitqt1-gui-actionbutton.h
    gui/polkitqt1-gui-actionbuttons.h

    core/polkitqt1-authority.h
    core/polkitqt1-details.h
    core/polkitqt1-identity.h
    core/polkitqt1-subject.h
    core/polkitqt1-temporaryauthorization.h
    core/polkitqt1-actiondescription.h

    agent/polkitqt1-agent-listener.h
    agent/polkitqt1-agent-session.h

    ${CMAKE_CURRENT_BINARY_DIR}/polkitqt1-version.h
    polkitqt1-export.h

    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1 COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Authority
    includes/PolkitQt1/Details
    includes/PolkitQt1/Identity
    includes/PolkitQt1/Subject
    includes/PolkitQt1/TemporaryAuthorization
    includes/PolkitQt1/ActionDescription
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt1 COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Gui/Action
    includes/PolkitQt1/Gui/ActionButton
    includes/PolkitQt1/Gui/ActionButtons
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt1/Gui COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Agent/Listener
    includes/PolkitQt1/Agent/Session
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt1/Agent COMPONENT Devel)

if(NOT WIN32)
    # Pkgconfig
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-core-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc
                   @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-gui-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui-1.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-agent-1.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent-1.pc
                   @ONLY)
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

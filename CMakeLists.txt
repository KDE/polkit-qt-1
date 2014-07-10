## Polkit Qt Library
project("PolkitQt-1")

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# Used to create CMake config files
include(CMakePackageConfigHelpers)

# Used to set installation paths
include(GNUInstallDirs)

# Set the different paths
set(LIB_DESTINATION         "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE PATH "Library directory name")
set(INCLUDE_DESTINATION     "${CMAKE_INSTALL_FULL_INCLUDEDIR}" CACHE PATH "The subdirectory to the header prefix")


# Check whether we explicitely select the Qt version to be used or autodetect
if(NOT USE_QT4 AND NOT USE_QT5)
    # Autodetect, prefering Qt 5
    message(STATUS "Autodetecting Qt version to use")
    find_package(Qt5Core QUIET)
    if(Qt5Core_FOUND)
        set(USE_QT5 TRUE)
    endif()
endif()

if(USE_QT5)
    message(STATUS "Using Qt 5")

    set(REQUIRED_QT_VERSION 5.1.0)

    find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core DBus Widgets)
    add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)

    set(POLKITQT-1_PCNAME "polkit-qt5-1")
    set(POLKITQT-1_CORE_PCNAME "polkit-qt5-core-1")
    set(POLKITQT-1_GUI_PCNAME "polkit-qt5-gui-1")
    set(POLKITQT-1_AGENT_PCNAME "polkit-qt5-agent-1")
    set(POLKITQT-1_CAMEL_NAME "PolkitQt5-1")
    set(POLKITQT-1_EXAMPLE "polkit-example-qt5")
    set(POLKITQT-1_EXAMPLE_HELPER "polkit-example-helper-qt5")
    set(POLKITQT-1_AGENT_EXAMPLE "polkit-agent-example-qt5")
    set(POLKITQT-1_INCLUDE_PATH "polkit-qt5-1")

    set(QT_QTCORE_LIBRARY Qt5::Core)
    set(QT_QTDBUS_LIBRARY Qt5::DBus)
    set(QT_QTTEST_LIBRARY Qt5::Test)
    set(QT_QTGUI_LIBRARY Qt5::Widgets)

    macro(qt4_wrap_ui)
        qt5_wrap_ui(${ARGN})
    endmacro()

    macro(qt4_add_resources)
        qt5_add_resources(${ARGN})
    endmacro()

    macro(qt4_add_dbus_adaptor)
        qt5_add_dbus_adaptor(${ARGN})
    endmacro()
else()
    message(STATUS "Using Qt 4")

    set(QT_MIN_VERSION "4.4.0")

    find_package(Qt4 REQUIRED)

    include (${QT_USE_FILE})
    include (InstallSettings)

    set(POLKITQT-1_PCNAME "polkit-qt-1")
    set(POLKITQT-1_CORE_PCNAME "polkit-qt-core-1")
    set(POLKITQT-1_GUI_PCNAME "polkit-qt-gui-1")
    set(POLKITQT-1_AGENT_PCNAME "polkit-qt-agent-1")
    set(POLKITQT-1_CAMEL_NAME "PolkitQt-1")
    set(POLKITQT-1_EXAMPLE "polkit-example")
    set(POLKITQT-1_EXAMPLE_HELPER "polkit-example-helper")
    set(POLKITQT-1_AGENT_EXAMPLE "polkit-agent-example")
    set(POLKITQT-1_INCLUDE_PATH "polkit-qt-1")
endif()

find_package(Polkit REQUIRED)
find_package(GObject REQUIRED)
find_package(GIO REQUIRED)

add_definitions(-DQT_NO_KEYWORDS)

include (CheckFunctionExists)

include_directories(
    ${POLKIT_INCLUDE_DIR}
    ${POLKIT_AGENT_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
    ${GOBJECT_INCLUDE_DIR}
    ${GIO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
if(NOT USE_QT5)
    include_directories(
        ${QT_QTXML_INCLUDE_DIR}
    )
endif()

# Check for older polkit
set(CMAKE_REQUIRED_INCLUDES ${POLKIT_INCLUDE_DIR} ${POLKIT_AGENT_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${POLKIT_LIBRARIES} ${POLKIT_AGENT_LIBRARY})
check_function_exists(polkit_agent_listener_register HAVE_POLKIT_AGENT_LISTENER_REGISTER)
check_function_exists(polkit_authority_get_sync HAVE_POLKIT_AUTHORITY_GET_SYNC)

if (NOT HAVE_POLKIT_AGENT_LISTENER_REGISTER OR NOT HAVE_POLKIT_AUTHORITY_GET_SYNC)
    message(STATUS "You have an older polkit-1 version: Polkit-Qt-1 will be built in compatibility mode")
    add_definitions(-DPOLKIT_QT_1_COMPATIBILITY_MODE)
endif (NOT HAVE_POLKIT_AGENT_LISTENER_REGISTER OR NOT HAVE_POLKIT_AUTHORITY_GET_SYNC)

option(USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR "Prefer to install the <package>Config.cmake files to lib/cmake/<package> instead of lib/<package>/cmake" TRUE)

set(POLKITQT-1_VERSION_MAJOR 0 CACHE INT "PolkitQt-1's major version number" FORCE)
set(POLKITQT-1_VERSION_MINOR 112 CACHE INT "PolkitQt-1's minor version number" FORCE)
set(POLKITQT-1_VERSION_PATCH 0 CACHE INT "PolkitQt-1's release version number" FORCE)
set(POLKITQT-1_VERSION_STRING
    "${POLKITQT-1_VERSION_MAJOR}.${POLKITQT-1_VERSION_MINOR}.${POLKITQT-1_VERSION_PATCH}")

# The SONUMBER of the polkit-qt-1 libraries
set(POLKITQT-1_ABI_VERSION 1)
# The version number of the polkit-qt-1 libraries
set(POLKITQT-1_LIBRARY_VERSION "${POLKITQT-1_ABI_VERSION}.${POLKITQT-1_VERSION_MINOR}.${POLKITQT-1_VERSION_PATCH}")
set(POLKITQT-1_LIB_NAMESPACE ${POLKITQT-1_CAMEL_NAME})

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
    ${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH} COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Authority
    includes/PolkitQt1/Details
    includes/PolkitQt1/Identity
    includes/PolkitQt1/Subject
    includes/PolkitQt1/TemporaryAuthorization
    includes/PolkitQt1/ActionDescription
    DESTINATION
    ${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}/PolkitQt1 COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Gui/Action
    includes/PolkitQt1/Gui/ActionButton
    includes/PolkitQt1/Gui/ActionButtons
    DESTINATION
    ${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}/PolkitQt1/Gui COMPONENT Devel)

install(FILES
    includes/PolkitQt1/Agent/Listener
    includes/PolkitQt1/Agent/Session
    DESTINATION
    ${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}/PolkitQt1/Agent COMPONENT Devel)

if(NOT WIN32)
    # Pkgconfig
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${POLKITQT-1_PCNAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_PCNAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_PCNAME}.pc DESTINATION ${LIB_DESTINATION}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${POLKITQT-1_CORE_PCNAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_CORE_PCNAME}.pc
                   @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_CORE_PCNAME}.pc DESTINATION ${LIB_DESTINATION}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${POLKITQT-1_GUI_PCNAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_GUI_PCNAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_GUI_PCNAME}.pc DESTINATION ${LIB_DESTINATION}/pkgconfig )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${POLKITQT-1_AGENT_PCNAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_AGENT_PCNAME}.pc
                   @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${POLKITQT-1_AGENT_PCNAME}.pc DESTINATION ${LIB_DESTINATION}/pkgconfig )
endif(NOT WIN32)


if(USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
   set(_PolkitQt-1Config_INSTALL_DIR ${LIB_DESTINATION}/cmake/${POLKITQT-1_CAMEL_NAME})
else(USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
   set(_PolkitQt-1Config_INSTALL_DIR ${LIB_DESTINATION}/${POLKITQT-1_CAMEL_NAME}/cmake)
endif(USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)


# Create the CMake Config files
configure_package_config_file(PolkitQt-1Config.cmake.in
                              ${CMAKE_BINARY_DIR}/${POLKITQT-1_CAMEL_NAME}Config.cmake
                              INSTALL_DESTINATION ${_PolkitQt-1Config_INSTALL_DIR}
                              PATH_VARS LIB_DESTINATION INCLUDE_DESTINATION
)

write_basic_package_version_file(${CMAKE_BINARY_DIR}/${POLKITQT-1_CAMEL_NAME}ConfigVersion.cmake
                                 VERSION ${POLKITQT-1_VERSION_STRING}
                                 COMPATIBILITY SameMajorVersion
                                )

install(FILES ${CMAKE_BINARY_DIR}/${POLKITQT-1_CAMEL_NAME}Config.cmake
              ${CMAKE_BINARY_DIR}/${POLKITQT-1_CAMEL_NAME}ConfigVersion.cmake
        DESTINATION ${_PolkitQt-1Config_INSTALL_DIR}
       )

install(EXPORT ${POLKITQT-1_CAMEL_NAME}Export FILE ${POLKITQT-1_CAMEL_NAME}Targets.cmake
        DESTINATION ${_PolkitQt-1Config_INSTALL_DIR}
        NAMESPACE ${POLKITQT-1_LIB_NAMESPACE}::)

option(BUILD_EXAMPLES "Builds a set of examples for polkit-qt-1" OFF)
if (BUILD_EXAMPLES)
    if(USE_QT5)
        find_package(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
        set(QT_QTXML_LIBRARY Qt5::Xml)
    endif()
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(cmake)

option(BUILD_TEST "Builds unit tests for polkit-qt-1" OFF)
if (BUILD_TEST)
    if(USE_QT5)
        find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
    endif()
    add_subdirectory(test)
endif (BUILD_TEST)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(agent)

# Always last!
include (PolkitQt-1Dist)

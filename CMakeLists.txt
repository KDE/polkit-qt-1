## Polkit Qt Library

cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

SET(QT_MIN_VERSION "4.4.0")

FIND_PACKAGE(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)

include (${QT_USE_FILE})
include (FindPkgConfig)
include (InstallSettings)

pkg_check_modules (POLKIT REQUIRED polkit>=0.8)
pkg_check_modules (POLKITDBUS REQUIRED polkit-dbus>=0.8)
pkg_check_modules (POLKITGRANT REQUIRED polkit-grant>=0.8)

MESSAGE(STATUS " ${POLKIT_INCLUDE_DIRS} ===========")

include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${POLKIT_INCLUDE_DIRS}
      ${POLKITDBUS_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

set(MAJOR_POLKIT_QT_VERSION "0")
set(MINOR_POLKIT_QT_VERSION "9")
set(PATCH_POLKIT_QT_VERSION "2")
set(POLKIT_QT_VERSION_STRING "${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}")

set(polkit_qt_core_SRCS
    auth.cpp
    context.cpp
)

set(polkit_qt_gui_SRCS
    action.cpp
    actionbutton.cpp
    actionbuttons.cpp
)

automoc4_add_library(polkit-qt-core SHARED ${polkit_qt_core_SRCS})
automoc4_add_library(polkit-qt-gui SHARED ${polkit_qt_gui_SRCS})

target_link_libraries(polkit-qt-core
 	${POLKITDBUS_LIBRARIES}
 	${POLKITGRANT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${QT_QTXML_LIBRARY}
	${POLKIT_LIBRARIES}
)

target_link_libraries(polkit-qt-gui
 	${POLKITDBUS_LIBRARIES}
 	${POLKITGRANT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${POLKIT_LIBRARIES}
)

set_target_properties(polkit-qt-core PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                      
set_target_properties(polkit-qt-gui PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                      
install(TARGETS polkit-qt-core ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS polkit-qt-gui ${INSTALL_TARGETS_DEFAULT_ARGS})


install(FILES
	action.h
	auth.h
	context.h
	actionbutton.h
	actionbuttons.h
    export.h
	includes/Action
	includes/Auth
	includes/Context
	includes/ActionButton
	includes/ActionButtons
	DESTINATION
	${INCLUDE_INSTALL_DIR}/PolicyKit/polkit-qt COMPONENT Devel)

# Pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-core.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-gui.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(cmake)

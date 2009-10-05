## Polkit Qt Library
project("Polkit-qt-1")

cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

SET(QT_MIN_VERSION "4.4.0")

FIND_PACKAGE(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)

include (${QT_USE_FILE})
include (FindPkgConfig)
include (InstallSettings)

pkg_check_modules (POLKIT REQUIRED polkit-gobject-1>=0.94)

MESSAGE(STATUS " ${POLKIT_INCLUDE_DIRS} ===========")

include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${POLKIT_INCLUDE_DIRS}
      ${POLKITDBUS_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/includes
      ${QT_QTXML_INCLUDE_DIR}
)

set(MAJOR_POLKIT_QT_VERSION "0")
set(MINOR_POLKIT_QT_VERSION "94")
set(PATCH_POLKIT_QT_VERSION "1")
set(POLKIT_QT_VERSION_STRING "${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}")

set(polkit_qt_core_SRCS
    authority.cpp
    identity.cpp
    subject.cpp
    temporaryauthorization.cpp
)

set(polkit_qt_gui_SRCS
    action.cpp
    actionbutton.cpp
    actionbuttons.cpp
)

set(polkit_qt_agent_SRCS
    session.cpp
    listener.cpp
    listeneradapter.cpp
    polkitqtlistener.cpp
)

automoc4_add_library(polkit-qt-core SHARED ${polkit_qt_core_SRCS})
automoc4_add_library(polkit-qt-gui SHARED ${polkit_qt_gui_SRCS})
automoc4_add_library(polkit-qt-agent SHARED ${polkit_qt_agent_SRCS})

target_link_libraries(polkit-qt-core
 	${POLKITDBUS_LIBRARIES}
 	${POLKITGRANT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${QT_QTXML_LIBRARY}
	${POLKIT_LIBRARIES}
)

target_link_libraries(polkit-qt-gui
	polkit-qt-core
 	${POLKITDBUS_LIBRARIES}
 	${POLKITGRANT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${POLKIT_LIBRARIES}
)

# FIXME polkit-agent-1
target_link_libraries(polkit-qt-agent
	${POLKIT_LIBRARIES}
        ${QT_QTCORE_LIBRARY}
        -lpolkit-agent-1
)

set_target_properties(polkit-qt-core PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                      
set_target_properties(polkit-qt-gui PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                    
set_target_properties(polkit-qt-agent PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
  
install(TARGETS polkit-qt-core ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS polkit-qt-gui ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS polkit-qt-agent ${INSTALL_TARGETS_DEFAULT_ARGS})


# FIXME all installable files
install(FILES
	action.h
	authority.h
	actionbutton.h
	actionbuttons.h
	export.h
	includes/Action
	includes/Authority
	includes/ActionButton
	includes/ActionButtons
	identity.h
	subject.h
	temporaryauthorization.h
	DESTINATION
	${INCLUDE_INSTALL_DIR}/PolicyKit/polkit-qt COMPONENT Devel)

# Pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-core.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-core.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-gui.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-gui.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polkit-qt-agent.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-agent.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig )

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(cmake)

if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)


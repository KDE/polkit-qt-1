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

pkg_check_modules (POLKIT REQUIRED polkit-gobject-1>=0.95)
pkg_check_modules (POLKIT_AGENT REQUIRED polkit-agent-1>=0.95)

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
set(MINOR_POLKIT_QT_VERSION "95")
set(PATCH_POLKIT_QT_VERSION "0")
set(POLKIT_QT_VERSION_STRING "${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}")

set(polkit_qt_core_SRCS
    authority.cpp
    identity.cpp
    subject.cpp
    temporaryauthorization.cpp
    details.cpp
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

automoc4_add_library(polkit-qt-core-1 SHARED ${polkit_qt_core_SRCS})
automoc4_add_library(polkit-qt-gui-1 SHARED ${polkit_qt_gui_SRCS})
automoc4_add_library(polkit-qt-agent-1 SHARED ${polkit_qt_agent_SRCS})

target_link_libraries(polkit-qt-core-1
	${QT_QTCORE_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${QT_QTXML_LIBRARY}
	${POLKIT_LIBRARIES}
)

target_link_libraries(polkit-qt-gui-1
	polkit-qt-core-1
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${POLKIT_LIBRARIES}
)

target_link_libraries(polkit-qt-agent-1
	${POLKIT_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${POLKIT_AGENT_LIBRARIES}
)

set_target_properties(polkit-qt-core-1 PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                      
set_target_properties(polkit-qt-gui-1 PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
                    
set_target_properties(polkit-qt-agent-1 PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
  
install(TARGETS polkit-qt-core-1 ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS polkit-qt-gui-1 ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS polkit-qt-agent-1 ${INSTALL_TARGETS_DEFAULT_ARGS})


install(FILES
	action.h
	authority.h
	actionbutton.h
	actionbuttons.h
        details.h
	identity.h
	subject.h
	temporaryauthorization.h
	export.h
	includes/Action
	includes/Authority
	includes/ActionButton
	includes/ActionButtons
        includes/Details
        includes/Identity
        includes/Subject
        includes/TemporaryAuthorization
	DESTINATION
	${INCLUDE_INSTALL_DIR}/polkit-qt-1/PolkitQt COMPONENT Devel)

install(FILES
	listener.h
	session.h
        export.h
	includes/Listener
	includes/Session
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

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

add_subdirectory(cmake)

if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)


include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
)

set(polkit_qt_gui_SRCS
    action.cpp
    actionbutton.cpp
    actionbuttons.cpp
)

automoc4_add_library(polkit-qt-gui-1 SHARED ${polkit_qt_gui_SRCS})

target_link_libraries(polkit-qt-gui-1
	polkit-qt-core-1
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${POLKIT_LIBRARIES}
)
                      
set_target_properties(polkit-qt-gui-1 PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )

install(TARGETS polkit-qt-gui-1 ${INSTALL_TARGETS_DEFAULT_ARGS})

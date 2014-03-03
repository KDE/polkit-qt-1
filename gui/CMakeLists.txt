set(polkit_qt_gui_SRCS
    polkitqt1-gui-action.cpp
    polkitqt1-gui-actionbutton.cpp
    polkitqt1-gui-actionbuttons.cpp
)

add_library(${POLKITQT-1_GUI_PCNAME} SHARED ${polkit_qt_gui_SRCS})

target_link_libraries(${POLKITQT-1_GUI_PCNAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${POLKIT_LIBRARIES}
    ${POLKITQT-1_CORE_PCNAME}
)

set_target_properties(${POLKITQT-1_GUI_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB)

install(TARGETS ${POLKITQT-1_GUI_PCNAME} ${INSTALL_TARGETS_DEFAULT_ARGS})

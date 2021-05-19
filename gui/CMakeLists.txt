set(polkit_qt_gui_SRCS
    polkitqt1-gui-action.cpp
    polkitqt1-gui-actionbutton.cpp
    polkitqt1-gui-actionbuttons.cpp
)

add_library(${POLKITQT-1_GUI_PCNAME} SHARED ${polkit_qt_gui_SRCS})

add_library(${POLKITQT-1_CAMEL_NAME}::Gui ALIAS ${POLKITQT-1_GUI_PCNAME})

generate_export_header(${POLKITQT-1_CORE_PCNAME}
    BASE_NAME polkitqt1-gui
    EXPORT_FILE_NAME polkitqt1-gui-export.h
)

target_link_libraries(${POLKITQT-1_GUI_PCNAME}
    PUBLIC
    ${POLKITQT-1_CORE_PCNAME}
    Qt5::Widgets
    PRIVATE
    Qt5::Core
    Qt5::DBus
)

set_target_properties(${POLKITQT-1_GUI_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB
                                EXPORT_NAME Gui)

target_include_directories(${POLKITQT-1_GUI_PCNAME}
    INTERFACE
        $<INSTALL_INTERFACE:${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(TARGETS ${POLKITQT-1_GUI_PCNAME} EXPORT ${POLKITQT-1_CAMEL_NAME}Export ARCHIVE DESTINATION ${LIB_DESTINATION}
                               LIBRARY DESTINATION ${LIB_DESTINATION}
                               RUNTIME DESTINATION bin)

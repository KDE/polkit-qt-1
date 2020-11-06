
install(FILES org.qt.policykit.examples.policy DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/polkit-1/actions/)

include_directories(
    ${CMAKE_SOURCE_DIR}/gui
)

set(polkit_example_SRCS
    main.cpp
    PkExample.cpp
)

SET(polkit_example_RESOUCES
    icons/icons.qrc
)

add_executable(${POLKITQT-1_EXAMPLE}
    ${polkit_example_SRCS}
    ${polkit_example_UI_SRCS}
    ${qtsourceview_RC_SRCS}
)

target_link_libraries(${POLKITQT-1_EXAMPLE}
    Qt5::Core
    Qt5::DBus
    Qt5::Widgets
    ${POLKITQT-1_GUI_PCNAME}
    ${POLKITQT-1_CORE_PCNAME}
)

#--------Helper Application

# This macro is defined in FindPolkitQt.cmake
macro(dbus_add_activation_system_service _sources)
    foreach (_i ${_sources})
        get_filename_component(_service_file ${_i} ABSOLUTE)
        string(REGEX REPLACE "\\.service.*$" ".service" _output_file ${_i})
        set(_target ${CMAKE_CURRENT_BINARY_DIR}/${_output_file})
        configure_file(${_service_file} ${_target})
        install(FILES ${_target} DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/dbus-1/system-services )
        #install(FILES ${_target} DESTINATION ${_install_dir})
    endforeach (_i ${ARGN})
endmacro(dbus_add_activation_system_service _sources)

set(polkit_example_helper_SRCS
    PkExampleHelper.cpp
    mainHelper.cpp
)

qt5_add_dbus_adaptor(polkit_example_helper_SRCS
    org.qt.policykit.examples.xml
    PkExampleHelper.h
    PkExampleHelper
)

add_executable(${POLKITQT-1_EXAMPLE_HELPER}
    ${polkit_example_helper_SRCS}
)

# see our helper is pretty small :D
target_link_libraries(${POLKITQT-1_EXAMPLE_HELPER}
    Qt5::Core
    Qt5::Xml
    Qt5::DBus
    ${POLKITQT-1_GUI_PCNAME}
)

install(TARGETS ${POLKITQT-1_EXAMPLE_HELPER} RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

dbus_add_activation_system_service(org.qt.policykit.examples.service.in)

install(FILES org.qt.policykit.examples.conf DESTINATION ${SYSCONF_INSTALL_DIR}/dbus-1/system.d)

add_subdirectory(agent)

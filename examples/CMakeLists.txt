
install(FILES org.qt.policykit.examples.policy DESTINATION ${SHARE_INSTALL_PREFIX}/polkit-1/actions/)

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

qt5_wrap_ui(polkit_example_UI_SRCS
    PkExample.ui
)

qt5_add_resources(qtsourceview_RC_SRCS  ${polkit_example_RESOUCES})

add_executable(polkit-example
    ${polkit_example_SRCS}
    ${polkit_example_UI_SRCS}
    ${qtsourceview_RC_SRCS}
)

target_link_libraries(polkit-example
    polkit-qt-gui-1
    polkit-qt-core-1
)

qt5_use_modules(polkit-example
    Core
)

#--------Helper Application

# This macro is defined in FindPolkitQt.cmake
macro(dbus_add_activation_system_service _sources)
    foreach (_i ${_sources})
        get_filename_component(_service_file ${_i} ABSOLUTE)
        string(REGEX REPLACE "\\.service.*$" ".service" _output_file ${_i})
        set(_target ${CMAKE_CURRENT_BINARY_DIR}/${_output_file})
        configure_file(${_service_file} ${_target})
        install(FILES ${_target} DESTINATION ${SHARE_INSTALL_PREFIX}/dbus-1/system-services )
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

add_executable(polkit-example-helper
    ${polkit_example_helper_SRCS}
)

# see our helper is pretty small :D
target_link_libraries(polkit-example-helper
    polkit-qt-core-1
)

qt5_use_modules(polkit-example-helper
    Core
)

install(TARGETS polkit-example-helper DESTINATION ${BIN_INSTALL_DIR})

dbus_add_activation_system_service(org.qt.policykit.examples.service.in)

install(FILES org.qt.policykit.examples.conf DESTINATION ${SYSCONF_INSTALL_DIR}/dbus-1/system.d)

add_subdirectory(agent)

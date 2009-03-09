
install(FILES org.qt.policykit.examples.policy DESTINATION ${SHARE_INSTALL_PREFIX}/PolicyKit/policy/)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(polkit_example_SRCS
    main.cpp
    PkExample.cpp
)

SET(polkit_example_RESOUCES
    icons/icons.qrc
)

QT4_WRAP_CPP(polkit_example_MOC_SRCS
    PkExample.h
)

QT4_WRAP_UI(polkit_example_UI_SRCS
    PkExample.ui
)

QT4_ADD_RESOURCES (qtsourceview_RC_SRCS  ${polkit_example_RESOUCES})

add_executable(polkit-example
    ${polkit_example_SRCS}
    ${polkit_example_MOC_SRCS}
    ${polkit_example_UI_SRCS}
    ${qtsourceview_RC_SRCS}
)

target_link_libraries(polkit-example
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    polkit-qt-gui
    polkit-qt-core
)

#--------Helper Application
set(polkit_example_helper_SRCS
    PkExampleHelper.cpp
    mainHelper.cpp
)

qt4_add_dbus_adaptor(polkit_example_helper_SRCS
    org.qt.policykit.examples.xml
    PkExampleHelper.h
    PkExampleHelper
)

QT4_WRAP_CPP(polkit_example_helper_MOC_SRCS
    PkExampleHelper.h
)

add_executable(polkit-example-helper
    ${polkit_example_helper_SRCS}
    ${polkit_example_helper_MOC_SRCS}
)

# see our helper is pretty small :D
target_link_libraries(polkit-example-helper
    ${QT_QTCORE_LIBRARY}
    polkit-qt-core
)

install(TARGETS polkit-example-helper DESTINATION ${LIBEXEC_INSTALL_DIR})


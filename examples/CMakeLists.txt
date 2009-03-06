
configure_file(org.qt.policykit.examples.policy.in ${CMAKE_CURRENT_BINARY_DIR}/org.qt.policykit.examples.policy)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.qt.policykit.examples.policy DESTINATION ${SHARE_INSTALL_PREFIX}/PolicyKit/policy/)

include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
)

    set(polkit_example_SRCS
        main.cpp
        PkExample.cpp
    )

    QT4_WRAP_CPP(polkit_example_MOC_SRCS
        PkExample.h
    )

    QT4_WRAP_UI(polkit_example_UI_SRCS
        PkExample.ui
    )

    add_executable(polkit-example
        ${polkit_example_SRCS}
        ${polkit_example_MOC_SRCS}
        ${polkit_example_UI_SRCS}
    )

    target_link_libraries(polkit-example ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} polkit-qt)


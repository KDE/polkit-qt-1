#install(FILES org.qt.policykit.examples.policy DESTINATION ${SHARE_INSTALL_PREFIX}/polkit-1/actions/)

FIND_PACKAGE(Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/agent
    ${QT_QTTEST_INCLUDE_DIR}
    ${POLKIT_INCLUDE_DIRS}
)

set(polkit_qt_test_SRCS
    test.cpp
)

QT4_WRAP_CPP(polkit_qt_test_MOC_SRCS
    test.h
)

add_executable(polkit-qt-test
    ${polkit_qt_test_SRCS}
    ${polkit_qt_test_MOC_SRCS}
)

target_link_libraries(polkit-qt-test
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    polkit-qt-core-1
)

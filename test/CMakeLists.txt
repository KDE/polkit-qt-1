enable_testing()

include_directories(
    ${CMAKE_SOURCE_DIR}/agent
)

add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    ${QT_CORE_LIBRARY}
    ${QT_DBUS_LIBRARY}
    ${QT_TEST_LIBRARY}
    ${QT_WIDGETS_LIBRARY}
    ${POLKITQT-1_CORE_PCNAME}
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

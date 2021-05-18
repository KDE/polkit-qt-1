enable_testing()

add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    Qt5::Core
    Qt5::DBus
    Qt5::Test
    Qt5::Widgets
    ${POLKITQT-1_CORE_PCNAME}
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

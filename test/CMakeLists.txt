enable_testing()

add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    Qt${QT_MAJOR_VERSION}::Test
    Qt${QT_MAJOR_VERSION}::Widgets
    ${POLKITQT-1_CORE_PCNAME}
    ${POLKITQT-1_AGENT_PCNAME}
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

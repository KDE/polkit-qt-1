enable_testing()

include_directories(
    ${CMAKE_SOURCE_DIR}/agent
)

add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    polkit-qt-core-1
)
qt5_use_modules(polkit-qt-test
    Core DBus Test
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

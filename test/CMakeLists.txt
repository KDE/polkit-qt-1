enable_testing()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/agent
)

automoc4_add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    polkit-qt-core-1
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

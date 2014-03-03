enable_testing()

include_directories(
    ${CMAKE_SOURCE_DIR}/agent
)

add_executable(polkit-qt-test
    test.cpp
)

target_link_libraries(polkit-qt-test
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${POLKITQT-1_CORE_PCNAME}
)

add_test(BaseTest ${CMAKE_CURRENT_BINARY_DIR}/polkit-qt-test)

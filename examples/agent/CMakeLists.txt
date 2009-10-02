include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(polkit_agent_example_SRCS
    main.cpp
    pkagentexample.cpp
    klistener.cpp
)

add_executable(polkit-agent-example
    ${polkit_agent_example_SRCS}
)

target_link_libraries(polkit-agent-example
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    polkit-qt-agent
    polkit-qt-core
)
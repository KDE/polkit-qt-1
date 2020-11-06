include_directories(
    ${CMAKE_SOURCE_DIR}/agent
)

set(polkit_agent_example_SRCS
    main.cpp
    pkagentexample.cpp
    klistener.cpp
)

add_executable(${POLKITQT-1_AGENT_EXAMPLE}
    ${polkit_agent_example_SRCS}
)

target_link_libraries(${POLKITQT-1_AGENT_EXAMPLE}
    ${QT_CORE_LIBRARY}
    ${QT_WIDGETS_LIBRARY}
    ${POLKITQT-1_AGENT_PCNAME}
    ${POLKITQT-1_CORE_PCNAME}
    PkgConfig::POLKIT_AGENT
    PkgConfig::GOBJECT
)

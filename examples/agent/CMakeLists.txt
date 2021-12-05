add_executable(${POLKITQT-1_AGENT_EXAMPLE})

target_sources(${POLKITQT-1_AGENT_EXAMPLE} PRIVATE
    main.cpp
    pkagentexample.cpp
    klistener.cpp
)

target_link_libraries(${POLKITQT-1_AGENT_EXAMPLE}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Widgets
    ${POLKITQT-1_AGENT_PCNAME}
    ${POLKITQT-1_CORE_PCNAME}
    PkgConfig::POLKIT_AGENT
    PkgConfig::GOBJECT
)

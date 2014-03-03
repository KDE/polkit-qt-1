set(polkit_qt_agent_SRCS
    polkitqt1-agent-session.cpp
    polkitqt1-agent-listener.cpp
    listeneradapter.cpp
    polkitqtlistener.cpp
)
add_library(${POLKITQT-1_AGENT_PCNAME} SHARED ${polkit_qt_agent_SRCS})

target_link_libraries(${POLKITQT-1_AGENT_PCNAME}
    ${POLKIT_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${POLKIT_AGENT_LIBRARY}
    ${POLKITQT-1_CORE_PCNAME}
)

set_target_properties(${POLKITQT-1_AGENT_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB)

install(TARGETS ${POLKITQT-1_AGENT_PCNAME} ${INSTALL_TARGETS_DEFAULT_ARGS})

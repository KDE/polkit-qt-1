add_library(${POLKITQT-1_AGENT_PCNAME} SHARED)
add_library(${POLKITQT-1_CAMEL_NAME}::Agent ALIAS ${POLKITQT-1_AGENT_PCNAME})

target_sources(${POLKITQT-1_AGENT_PCNAME} PRIVATE
    polkitqt1-agent-session.cpp
    polkitqt1-agent-listener.cpp
    listeneradapter.cpp
    polkitqtlistener.cpp
)
generate_export_header(${POLKITQT-1_AGENT_PCNAME}
    BASE_NAME polkitqt1-agent
    EXPORT_FILE_NAME polkitqt1-agent-export.h
)

target_link_libraries(${POLKITQT-1_AGENT_PCNAME}
    PUBLIC
    Qt5::Core
    ${POLKITQT-1_CORE_PCNAME}
    PRIVATE
    Qt5::Core
    PkgConfig::POLKIT_AGENT
    PkgConfig::GOBJECT
)

set_target_properties(${POLKITQT-1_AGENT_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB
                                EXPORT_NAME Agent)

target_include_directories(${POLKITQT-1_AGENT_PCNAME}
    INTERFACE
        $<INSTALL_INTERFACE:${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(TARGETS ${POLKITQT-1_AGENT_PCNAME} EXPORT ${POLKITQT-1_CAMEL_NAME}Export ARCHIVE DESTINATION ${LIB_DESTINATION}
                               LIBRARY DESTINATION ${LIB_DESTINATION}
                               RUNTIME DESTINATION bin)

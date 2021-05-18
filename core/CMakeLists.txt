set(polkit_qt_core_SRCS
    polkitqt1-authority.cpp
    polkitqt1-identity.cpp
    polkitqt1-subject.cpp
    polkitqt1-temporaryauthorization.cpp
    polkitqt1-details.cpp
    polkitqt1-actiondescription.cpp
)

add_library(${POLKITQT-1_CORE_PCNAME} SHARED ${polkit_qt_core_SRCS})

add_library(${POLKITQT-1_CAMEL_NAME}::Core ALIAS ${POLKITQT-1_CORE_PCNAME})

target_link_libraries(${POLKITQT-1_CORE_PCNAME}
    PUBLIC
    Qt5::Core
    PRIVATE
    Qt5::DBus
    PkgConfig::POLKIT_GOBJECT
    PkgConfig::GLIB2
    PkgConfig::GOBJECT
)

set_target_properties(${POLKITQT-1_CORE_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB
                                EXPORT_NAME Core)

target_include_directories(${POLKITQT-1_CORE_PCNAME} INTERFACE $<INSTALL_INTERFACE:${INCLUDE_DESTINATION}/${POLKITQT-1_INCLUDE_PATH}>)

install(TARGETS ${POLKITQT-1_CORE_PCNAME} EXPORT ${POLKITQT-1_CAMEL_NAME}Export ARCHIVE DESTINATION ${LIB_DESTINATION}
                               LIBRARY DESTINATION ${LIB_DESTINATION}
                               RUNTIME DESTINATION bin)


set(polkit_qt_core_SRCS
    polkitqt1-authority.cpp
    polkitqt1-identity.cpp
    polkitqt1-subject.cpp
    polkitqt1-temporaryauthorization.cpp
    polkitqt1-details.cpp
    polkitqt1-actiondescription.cpp
)

add_library(${POLKITQT-1_CORE_PCNAME} SHARED ${polkit_qt_core_SRCS})

target_link_libraries(${POLKITQT-1_CORE_PCNAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${POLKIT_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GIO_LIBRARIES}
)

set_target_properties(${POLKITQT-1_CORE_PCNAME} PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB)

install(TARGETS ${POLKITQT-1_CORE_PCNAME} ${INSTALL_TARGETS_DEFAULT_ARGS})

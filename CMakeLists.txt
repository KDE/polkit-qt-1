## Polkit Qt Library

set(polkit_qt_LIB_SRCS
    polkit_qt_action.cpp
    polkit_qt_auth.cpp
    polkit_qt_context.cpp
)

kde4_add_library(polkit_qt SHARED ${polkit_qt_LIB_SRCS})

target_link_libraries(polkit_qt
# 	${POLKITDBUS_LIBRARIES}
# 	${POLKITGRANT_LIBRARIES}
	${POLKIT_LIBRARIES}
)

set_target_properties(polkit_qt PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS polkit_qt  ${INSTALL_TARGETS_DEFAULT_ARGS})

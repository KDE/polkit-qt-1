include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(polkit_qt_core_SRCS
    polkitqt1-authority.cpp
    polkitqt1-identity.cpp
    polkitqt1-subject.cpp
    polkitqt1-temporaryauthorization.cpp
    polkitqt1-details.cpp
    polkitqt1-actiondescription.cpp
)

automoc4_add_library(polkit-qt-core-1 SHARED ${polkit_qt_core_SRCS})

target_link_libraries(polkit-qt-core-1
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${POLKIT_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GIO_LIBRARIES}
)

set_target_properties(polkit-qt-core-1 PROPERTIES VERSION ${POLKITQT-1_LIBRARY_VERSION}
                                SOVERSION ${POLKITQT-1_ABI_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB)

install(TARGETS polkit-qt-core-1 ${INSTALL_TARGETS_DEFAULT_ARGS})

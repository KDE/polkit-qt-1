include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
)

set(polkit_qt_core_SRCS
    authority.cpp
    identity.cpp
    subject.cpp
    temporaryauthorization.cpp
    details.cpp
    actiondescription.cpp
)

automoc4_add_library(polkit-qt-core-1 SHARED ${polkit_qt_core_SRCS})

target_link_libraries(polkit-qt-core-1
	${QT_QTCORE_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${QT_QTXML_LIBRARY}
	${POLKIT-1_LIBRARIES}
)

set_target_properties(polkit-qt-core-1 PROPERTIES VERSION ${CMAKE_POLKITQT-1_VERSION_STRING}
                                SOVERSION ${CMAKE_MAJOR_POLKITQT-1_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB
                      )
  
install(TARGETS polkit-qt-core-1 ${INSTALL_TARGETS_DEFAULT_ARGS})

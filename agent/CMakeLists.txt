include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
)

set(MAJOR_POLKITQT1_VERSION "0")
set(MINOR_POLKITQT1_VERSION "95")
set(PATCH_POLKITQT1_VERSION "0")
set(POLKITQT1_VERSION_STRING "${MAJOR_POLKITQT1_VERSION}.${MINOR_POLKITQT1_VERSION}.${PATCH_POLKITQT1_VERSION}")

set(polkit_qt_agent_SRCS
    session.cpp
    listener.cpp
    listeneradapter.cpp
    polkitqtlistener.cpp
)
automoc4_add_library(polkit-qt-agent-1 SHARED ${polkit_qt_agent_SRCS})

target_link_libraries(polkit-qt-agent-1
	${POLKIT_LIBRARIES}
        ${QT_QTCORE_LIBRARY}
        ${POLKIT_AGENT_LIBRARIES}
	polkit-qt-core-1
)

set_target_properties(polkit-qt-agent-1 PROPERTIES VERSION ${MAJOR_POLKITQT1_VERSION}.${MINOR_POLKITQT1_VERSION}.${PATCH_POLKITQT1_VERSION}
                                SOVERSION ${MAJOR_POLKITQT1_VERSION}
                                DEFINE_SYMBOL MAKE_POLKITQT1_LIB
                      )
  
install(TARGETS polkit-qt-agent-1 ${INSTALL_TARGETS_DEFAULT_ARGS}) 

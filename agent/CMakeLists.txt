include_directories(
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
)

set(MAJOR_POLKIT_QT_VERSION "0")
set(MINOR_POLKIT_QT_VERSION "95")
set(PATCH_POLKIT_QT_VERSION "0")
set(POLKIT_QT_VERSION_STRING "${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}")

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
)

set_target_properties(polkit-qt-agent-1 PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                                DEFINE_SYMBOL MAKE_POLKIT_QT_LIB
                      )
  
install(TARGETS polkit-qt-agent-1 ${INSTALL_TARGETS_DEFAULT_ARGS}) 
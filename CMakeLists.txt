## Polkit Qt Library

set(MAJOR_POLKIT_QT_VERSION "0")
set(MINOR_POLKIT_QT_VERSION "9")
set(PATCH_POLKIT_QT_VERSION "0")

set(polkit_qt_LIB_SRCS
    action.cpp
    auth.cpp
    context.cpp
    actionbutton.cpp
)

SET(polkit_qt_MOC_HDRS
    action.h
    auth.h
    context.h
    actionbutton.h
)

QT4_WRAP_CPP(polkit_qt_MOC_SRCS ${polkit_qt_MOC_HDRS})
QT4_AUTOMOC(${polkit_qt_SRCS})

add_library(polkit-qt SHARED ${polkit_qt_LIB_SRCS} ${polkit_qt_MOC_SRCS})

target_link_libraries(polkit-qt
 	${POLKITDBUS_LIBRARIES}
 	${POLKITGRANT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTDBUS_LIBRARY}
	${QT_QTXML_LIBRARY}
	${POLKIT_LIBRARIES}
)

set_target_properties(polkit-qt PROPERTIES VERSION ${MAJOR_POLKIT_QT_VERSION}.${MINOR_POLKIT_QT_VERSION}.${PATCH_POLKIT_QT_VERSION}
                                      SOVERSION ${MAJOR_POLKIT_QT_VERSION}
                      )
                      
install(TARGETS polkit-qt  ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	includes/Action
	includes/Auth
	includes/Context
	includes/ActionButton
	DESTINATION
	${INCLUDE_INSTALL_DIR}/PolicyKit/polkit-qt)

add_subdirectory(cmake)

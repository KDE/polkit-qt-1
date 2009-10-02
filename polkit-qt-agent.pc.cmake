prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: Polkit-qt
Description: Convenience library for using PolicyKit Agent with a Qt-styled API, GUI functions
Version: @POLKIT_QT_VERSION_STRING@
Requires: QtCore QtGui polkit-qt-agent
Libs: -L${libdir} -lpolkit-qt-agent
Cflags: -I${includedir}

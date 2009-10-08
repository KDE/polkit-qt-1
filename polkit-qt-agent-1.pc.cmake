prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: Polkit-qt-1
Description: Convenience library for using PolicyKit Agent with a Qt-styled API
Version: @POLKIT_QT_VERSION_STRING@
Requires: QtCore QtGui polkit-qt-agent-1
Libs: -L${libdir} -lpolkit-qt-agent-1
Cflags: -I${includedir}

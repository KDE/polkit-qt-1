prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: Polkit-qt
Description: Convenience library for using PolicyKit with a Qt-styled API, non-GUI functions
Version: @POLKIT_QT_VERSION_STRING@
Requires: QtCore
Libs: -L${libdir} -lpolkit-qt-core
Cflags: -I${includedir}

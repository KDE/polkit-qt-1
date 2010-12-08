prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: polkit-qt-gui-1
Description: Convenience library for using polkit with a Qt-styled API, GUI classes
Version: @POLKITQT-1_VERSION_STRING@
Requires: QtCore QtGui polkit-qt-core-1
Libs: -L${libdir} -lpolkit-qt-gui-1
Cflags: -I${includedir}

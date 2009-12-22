prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: polkit-qt-core-1
Description: Convenience library for using polkit with a Qt-styled API, non-GUI classes
Version: @POLKITQT1_VERSION_STRING@
Requires: QtCore
Libs: -L${libdir} -lpolkit-qt-core-1
Cflags: -I${includedir}

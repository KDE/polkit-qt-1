prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: polkit-qt-1
Description: Convenience library for using polkit with a Qt-styled API
Version: @CMAKE_POLKITQT-1_VERSION_STRING@
Requires: polkit-qt-core-1 polkit-qt-gui-1 polkit-qt-agent-1
Libs: -L${libdir} -lpolkit-qt-core-1 -lpolkit-qt-gui-1 -lpolkit-qt-agent-1
Cflags: -I${includedir}

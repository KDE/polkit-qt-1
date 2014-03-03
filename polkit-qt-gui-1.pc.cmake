prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: @POLKITQT-1_GUI_PCNAME@
Description: Convenience library for using polkit with a Qt-styled API, GUI classes
Version: @POLKITQT-1_VERSION_STRING@
Requires: QtCore QtGui @POLKITQT-1_CORE_PCNAME@
Libs: -L${libdir} -l@POLKITQT-1_GUI_PCNAME@
Cflags: -I${includedir}/@POLKITQT-1_INCLUDE_PATH@

prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_INSTALL_DIR@
includedir=@CMAKE_INSTALL_PREFIX@/include

Name: @POLKITQT-1_AGENT_PCNAME@
Description: Convenience library for using polkit Agent with a Qt-styled API
Version: @POLKITQT-1_VERSION_STRING@
Requires: QtCore QtGui
Libs: -L${libdir} -l@POLKITQT-1_AGENT_PCNAME@
Cflags: -I${includedir}/@POLKITQT-1_INCLUDE_PATH@

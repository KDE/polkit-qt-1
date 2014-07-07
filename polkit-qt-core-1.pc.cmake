prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_DESTINATION@
includedir=@INCLUDE_DESTINATION@/@POLKITQT-1_INCLUDE_PATH@

Name: @POLKITQT-1_CORE_PCNAME@
Description: Convenience library for using polkit with a Qt-styled API, non-GUI classes
Version: @POLKITQT-1_VERSION_STRING@
Requires: QtCore
Libs: -L${libdir} -l@POLKITQT-1_CORE_PCNAME@
Cflags: -I${includedir}

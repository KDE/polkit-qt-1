prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=${exec_prefix}/@CMAKE_INSTALL_LIBDIR@
includedir=${prefix}/@CMAKE_INSTALL_INCLUDEDIR@/@POLKITQT-1_INCLUDE_PATH@

Name: @POLKITQT-1_CORE_PCNAME@
Description: Convenience library for using polkit with a Qt-styled API, non-GUI classes
Version: @POLKITQT-1_VERSION_STRING@
Requires: Qt@QT_MAJOR_VERSION@Core
Libs: -L${libdir} -l@POLKITQT-1_CORE_PCNAME@
Cflags: -I${includedir}

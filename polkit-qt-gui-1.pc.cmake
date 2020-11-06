prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_DESTINATION@
includedir=@INCLUDE_DESTINATION@/@POLKITQT-1_INCLUDE_PATH@

Name: @POLKITQT-1_GUI_PCNAME@
Description: Convenience library for using polkit with a Qt-styled API, GUI classes
Version: @POLKITQT-1_VERSION_STRING@
Requires: @QT_CORE_PCNAME@ @QT_GUI_PCNAME@ @POLKITQT-1_CORE_PCNAME@
Libs: -L${libdir} -l@POLKITQT-1_GUI_PCNAME@
Cflags: -I${includedir}

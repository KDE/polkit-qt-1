prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@LIB_DESTINATION@
includedir=@INCLUDE_DESTINATION@/@POLKITQT-1_INCLUDE_PATH@

Name: @POLKITQT-1_AGENT_PCNAME@
Description: Convenience library for using polkit Agent with a Qt-styled API
Version: @POLKITQT-1_VERSION_STRING@
Requires: @QT_CORE_PCNAME@ @QT_GUI_PCNAME@
Libs: -L${libdir} -l@POLKITQT-1_AGENT_PCNAME@
Cflags: -I${includedir}

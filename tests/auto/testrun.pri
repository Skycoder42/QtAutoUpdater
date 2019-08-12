win32:!ReleaseBuild:!DebugBuild {
	runtarget.target = run-tests
	runtarget.CONFIG = recursive
	runtarget.recurse_target = run-tests
	QMAKE_EXTRA_TARGETS += runtarget
} else {
	oneshell.target = .ONESHELL
	QMAKE_EXTRA_TARGETS += oneshell

	win32:!win32-g++ {
		CONFIG(debug, debug|release): outdir_helper = debug
		CONFIG(release, debug|release): outdir_helper = release
		runtarget.target = run-tests
		!compat_test: runtarget.depends += $(DESTDIR_TARGET)
		runtarget.commands += @set PATH=$$shell_path($$shadowed($$dirname(_QMAKE_CONF_))/bin);$$shell_path($$[QT_INSTALL_BINS]);$(PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@set QT_PLUGIN_PATH=$$shadowed($$dirname(_QMAKE_CONF_))/plugins;$(QT_PLUGIN_PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@set QML2_IMPORT_PATH=$$shadowed($$dirname(_QMAKE_CONF_))/qml;$(QML2_IMPORT_PATH)
		runtarget.commands += $$escape_expand(\\n\\t)@if exist $${outdir_helper}\\fail del $${outdir_helper}\\fail
		runtarget.commands += $$escape_expand(\\n\\t)start /w call $(DESTDIR_TARGET) ^> $${outdir_helper}\\test.log ^|^| echo FAIL ^> $${outdir_helper}\\fail ^& exit 0
		runtarget.commands += $$escape_expand(\\n\\t)type $${outdir_helper}\\test.log
		runtarget.commands += $$escape_expand(\\n\\t)@if exist $${outdir_helper}\\fail exit 42
		QMAKE_EXTRA_TARGETS += runtarget
	} else {
		win32-g++: QMAKE_DIRLIST_SEP = ";"
		runtarget.commands += @export PATH=\"$$shell_path($$shadowed($$dirname(_QMAKE_CONF_))/bin/):$$shell_path($$[QT_INSTALL_BINS]):$${LITERAL_DOLLAR}$${LITERAL_DOLLAR}PATH\"
		runtarget.commands += $$escape_expand(\\n\\t)@export QT_PLUGIN_PATH=\"$$shadowed($$dirname(_QMAKE_CONF_))/plugins/$${QMAKE_DIRLIST_SEP}$(QT_PLUGIN_PATH)\"
		runtarget.commands += $$escape_expand(\\n\\t)@export QML2_IMPORT_PATH=\"$$shadowed($$dirname(_QMAKE_CONF_))/qml/$${QMAKE_DIRLIST_SEP}$(QML2_IMPORT_PATH)\"
		win32-g++: QMAKE_DIRLIST_SEP = ":"

		linux|win32-g++ {
			runtarget.commands += $$escape_expand(\\n\\t)@export LD_LIBRARY_PATH=\"$$shadowed($$dirname(_QMAKE_CONF_))/lib/$${QMAKE_DIRLIST_SEP}$$[QT_INSTALL_LIBS]$${QMAKE_DIRLIST_SEP}$(LD_LIBRARY_PATH)\"
			runtarget.commands += $$escape_expand(\\n\\t)@export QT_QPA_PLATFORM=minimal
		} else:mac {
			runtarget.commands += $$escape_expand(\\n\\t)@export DYLD_LIBRARY_PATH=\"$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_LIBRARY_PATH)\"
			runtarget.commands += $$escape_expand(\\n\\t)@export DYLD_FRAMEWORK_PATH=\"$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$(DYLD_FRAMEWORK_PATH)\"
		}

		runtarget.target = run-tests
		win32-g++ {
			!compat_test: runtarget.depends += $(DESTDIR_TARGET)
			runtarget.commands += $$escape_expand(\\n\\t)./$(DESTDIR_TARGET)
		} else {
			!compat_test: runtarget.depends += $(TARGET)
			runtarget.commands += $$escape_expand(\\n\\t)./$(TARGET)
		}
		QMAKE_EXTRA_TARGETS += runtarget
	}
}

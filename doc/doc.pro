TEMPLATE = aux

OTHER_FILES += Doxyfile \
	makedoc.sh \
	doxme.py \
	../README.md \
	*.dox \
	snippets/*.cpp \
	images/*

system($$QMAKE_MKDIR $$shell_quote($$shell_path($$OUT_PWD/qtautoupdater)))

docTarget.target = doxygen
docTarget.commands = $$PWD/makedoc.sh "$$PWD" "$$MODULE_VERSION" "$$[QT_INSTALL_BINS]" "$$[QT_INSTALL_HEADERS]" "$$[QT_INSTALL_DOCS]"
QMAKE_EXTRA_TARGETS += docTarget

docInst1.path = $$[QT_INSTALL_DOCS]
docInst1.files = $$OUT_PWD/qtautoupdater.qch
docInst1.CONFIG += no_check_exist
docInst2.path = $$[QT_INSTALL_DOCS]
docInst2.files = $$OUT_PWD/qtautoupdater
INSTALLS += docInst1 docInst2

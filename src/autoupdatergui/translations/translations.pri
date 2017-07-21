TRANSLATIONS += $$PWD/qtautoupdatergui_de.ts \
	$$PWD/qtautoupdatergui_template.ts

OTHER_FILES += $$PWD/qtautoupdatergui_de.ts \
	$$PWD/qtautoupdatergui_template.ts

qtPrepareTool(LRELEASE, lrelease)
LRELEASE += -nounfinished

releaseTarget.target = lrelease
releaseTarget.commands = $$LRELEASE "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/qtautoupdatergui_de.qm \
	$$PWD/qtautoupdatergui_template.ts
trInstall.CONFIG += no_check_exist
trInstall.depends = releaseTarget
INSTALLS += trInstall

TRANSLATIONS += $$PWD/qtautoupdatergui_de.ts \
	$$PWD/qtautoupdatergui_template.ts

OTHER_FILES += $$PWD/qtautoupdatergui_de.ts \
	$$PWD/qtautoupdatergui_template.ts

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$OUT_PWD/qtautoupdatergui_de.qm \
	$$PWD/qtautoupdatergui_template.ts
trInstall.CONFIG += no_check_exist
trInstall.depends = lrelease
INSTALLS += trInstall

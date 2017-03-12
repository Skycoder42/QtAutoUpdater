TRANSLATIONS += $$PWD/QtAutoUpdaterController_de.ts \
	$$PWD/QtAutoUpdaterController_template.ts

OTHER_FILES += $$PWD/QtAutoUpdaterController_de.ts \
	$$PWD/QtAutoUpdaterController_template.ts

updateTarget.target = lupdate
updateTarget.commands = lupdate -no-obsolete -locations relative "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += updateTarget

releaseTarget.target = lrelease
releaseTarget.commands = lrelease -compress -nounfinished "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/*.qm
INSTALLS += trInstall

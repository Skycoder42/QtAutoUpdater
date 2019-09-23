TEMPLATE = aux

QDEP_LUPDATE_INPUTS += $$PWD/../autoupdatercore
QDEP_LUPDATE_INPUTS += $$PWD/../autoupdaterwidgets
QDEP_LUPDATE_INPUTS += $$PWD/../plugins
QDEP_LUPDATE_INPUTS += $$PWD/../imports
QDEP_LUPDATE_INPUTS += $$PWD/../jar

TRANSLATIONS += \
	qtautoupdater_de.ts \
	qtautoupdater_es.ts \
	qtautoupdater_fr.ts \
	qtautoupdater_ar.ts \
	qtautoupdater_template.ts

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

QDEP_DEPENDS += \
	Skycoder42/DialogMaster@1.4.0 \
	Skycoder42/QTaskbarControl@1.3.2

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtautoupdater_template.qm
QM_FILES += qtautoupdater_template.ts

HEADERS =
SOURCES =
GENERATED_SOURCES =
OBJECTIVE_SOURCES =
RESOURCES =

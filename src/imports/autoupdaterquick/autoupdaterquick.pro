QT = core qml autoupdatercore quick
CXX_MODULE = autoupdaterquick
TARGETPATH = de/skycoder42/QtAutoUpdater/Quick
TARGET  = declarative_autoupdaterquick
IMPORT_VERSION = $$MODULE_VERSION_IMPORT
DEFINES += "VERSION_MAJOR=$$MODULE_VERSION_MAJOR"
DEFINES += "VERSION_MINOR=$$MODULE_VERSION_MINOR"

HEADERS += \
	qmlautoupdaterquickhelper.h \
	qmlupdateinfomodel.h \
	qtautoupdaterquick_plugin.h

SOURCES += \
	qmlautoupdaterquickhelper.cpp \
	qmlupdateinfomodel.cpp \
	qtautoupdaterquick_plugin.cpp

QML_FILES += \
	DialogBase.qml \
	ProgressItem.qml \
	ProgressDialog.qml \
	AskUpdateDialog.qml \
	UpdateResultDialog.qml \
	UpdaterComponent.qml \
	UpdateInfoPage.qml \
	UpdateInfoComponent.qml \
	SelectComponentView.qml \
	InstallView.qml \
	UpdateInstallerPage.qml \
	UpdateInstallerComponent.qml \
	SuccessView.qml \
	UpdateButton.qml\
	UpdateAction.qml

RESOURCES += \
	autoupdaterquick.qrc

OTHER_FILES += \
	qmldir

CONFIG += qmlcache
load(qml_plugin)

generate_qmltypes {
	# run again to overwrite module env
	ldpath.name = LD_LIBRARY_PATH
	ldpath.value = "$$shadowed($$dirname(_QMAKE_CONF_))/lib/:$$[QT_INSTALL_LIBS]:$$(LD_LIBRARY_PATH)"
	qmlpath.name = QML2_IMPORT_PATH
	qmlpath.value = "$$shadowed($$dirname(_QMAKE_CONF_))/qml/:$$[QT_INSTALL_QML]:$$(QML2_IMPORT_PATH)"
	plgpath.name = QT_PLUGIN_PATH
	plgpath.value = "$$shadowed($$dirname(_QMAKE_CONF_))/plugins/:$$[QT_INSTALL_PLUGINS]:$$(QT_PLUGIN_PATH)"
	PLGDUMP_ENV = ldpath qmlpath plgpath
	QT_TOOL_ENV = ldpath qmlpath plgpath
	qtPrepareTool(QMLPLUGINDUMP, qmlplugindump)
	QT_TOOL_ENV =

	#overwrite the target deps as make target is otherwise not detected
	qmltypes.depends = ../../../qml/$$TARGETPATH/$(TARGET)
	OLDDMP = $$take_first(qmltypes.commands)
	qmltypes.commands = $$QMLPLUGINDUMP $${qmltypes.commands}
	message("replaced $$OLDDMP with $$QMLPLUGINDUMP")

	mfirst.target = all
	mfirst.depends += qmltypes
	QMAKE_EXTRA_TARGETS += mfirst
}

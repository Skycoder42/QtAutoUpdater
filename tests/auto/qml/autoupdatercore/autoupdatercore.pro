TEMPLATE = app

QT = core qml autoupdatercore
CONFIG += qmltestcase console

TARGET = tst_qmlautoupdatercore

importFiles.path = .
DEPLOYMENT += importFiles

DISTFILES += \
	test.conf \
	tst_qmlautoupdatercore.qml

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../../testrun.pri)

SOURCES += \
	tst_qmlautoupdatercore.cpp

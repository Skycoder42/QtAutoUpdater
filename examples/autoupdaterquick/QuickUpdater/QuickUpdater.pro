TEMPLATE = app

QT += quick autoupdatercore

TARGET = QuickUpdater

SOURCES += \
		main.cpp

RESOURCES += qml.qrc

DISTFILES += \
	example.conf

DEFINES += "EXAMPLE_PATH=\\\"$$PWD/example.conf\\\""

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdaterquick/QuickUpdater
!install_ok: INSTALLS += target

TEMPLATE = app

QT += core autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = ConsoleUpdater

SOURCES += main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdatercore/ConsoleUpdater
!install_ok: INSTALLS += target

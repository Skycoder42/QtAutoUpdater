#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:59:22
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WidgetsTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterWidgets/release/ -lAutoUpdaterWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterWidgets/debug/ -lAutoUpdaterWidgetsd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterWidgets/ -lAutoUpdaterWidgets
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterWidgets/ -lAutoUpdaterWidgets_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterWidgets/ -lAutoUpdaterWidgets

INCLUDEPATH += $$PWD/../../AutoUpdaterWidgets
DEPENDPATH += $$PWD/../../AutoUpdaterWidgets

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/release/libAutoUpdaterWidgets.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/debug/libAutoUpdaterWidgetsd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/release/AutoUpdaterWidgets.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/debug/AutoUpdaterWidgetsd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/libAutoUpdaterWidgets.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/libAutoUpdaterWidgets_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterWidgets/libAutoUpdaterWidgets.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/release/ -lAutoUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/debug/ -lAutoUpdaterd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/libAutoUpdater.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/libAutoUpdaterd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/AutoUpdater.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/AutoUpdaterd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater.a

RESOURCES += \
    main_res.qrc

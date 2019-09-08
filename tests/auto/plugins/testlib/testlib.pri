win32:CONFIG(release, debug|release): LIBS += -L$$shadowed($$PWD)/release/ -ltestlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$shadowed($$PWD)/debug/ -ltestlib
else:unix: LIBS += -L$$shadowed($$PWD)/ -ltestlib

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$shadowed($$PWD)/release/libtestlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$shadowed($$PWD)/debug/libtestlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$shadowed($$PWD)/release/testlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$shadowed($$PWD)/debug/testlib.lib
else:unix: PRE_TARGETDEPS += $$shadowed($$PWD)/libtestlib.a

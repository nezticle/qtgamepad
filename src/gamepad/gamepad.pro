TARGET     = QtGamepad
QT         = core

LIBS += -ludev

load(qt_module)

HEADERS += \
    qtgamepadglobal.h \
    qgamepaddevicediscovery_p.h \
    qgamepadmanager.h \
    qgamepadhandler.h \
    qgamepadinputstate.h \
    qgamepadkeybindings.h
SOURCES += \
    qgamepaddevicediscovery.cpp \
    qgamepadmanager.cpp \
    qgamepadhandler.cpp \
    qgamepadinputstate.cpp \
    qgamepadkeybindings.cpp

QT += gui
TEMPLATE = lib

CONFIG += staticlib
TARGET = qtgamepad

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

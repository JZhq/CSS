#-------------------------------------------------
#
# Project created by QtCreator 2018-04-27T22:16:29
#
#-------------------------------------------------
include(../include/global.prf)
QT       -= gui
QT       += widgets axcontainer

TARGET = util
TEMPLATE = lib

CONFIG += C++11

DEFINES += UTIL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    util.cpp \
    uiutil.cpp \
    json.cpp \
    config.cpp \
    ninepatchpainter.cpp \
    loghandler.cpp

HEADERS += \
    util.h \
    util_global.h \
    uiutil.h \
    json.h \
    config.h \
    ninepatchpainter.h \
    loghandler.h

CONFIG(debug, debug|release) {
    TARGET = utild
    DESTDIR = $$DLL_DIR_DEBUG
}
CONFIG(release, debug|release) {
    TARGET = util
    DESTDIR = $$DLL_DIR_RELEASE
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

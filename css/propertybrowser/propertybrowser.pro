#-------------------------------------------------
#
# Project created by QtCreator 2014-12-01T17:14:08
#
#-------------------------------------------------

include(../include/global.prf)
include(src/qtpropertybrowser.pri)
include(../include/public.prf)
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core-private

TEMPLATE = lib

CONFIG(debug, debug|release) {
    TARGET = propertybrowserd
    DESTDIR = $$DLL_DIR_DEBUG
}
CONFIG(release, debug|release) {
    TARGET = propertybrowser
    DESTDIR = $$DLL_DIR_RELEASE
}

DEFINES += PROPERTYBROWSER_LIBRARY

SOURCES += \
    groupboxpropertybrowser.cpp \
    propertyconstants.cpp \
    treepropertybrowser.cpp \
    src/qthistoricalrecordlineedit.cpp \
    src/qthistoricalrecordtextedit.cpp

HEADERS +=\
    groupboxpropertybrowser.h \
    propertybrowser_global.h \
    propertyconstants.h \
    treepropertybrowser.h \
    src/qthistoricalrecordlineedit.h \
    src/qthistoricalrecordtextedit.h

TRANSLATIONS += zh_CN.ts

RESOURCES += \
    resource.qrc

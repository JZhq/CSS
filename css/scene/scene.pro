include(../include/global.prf)
include(../include/public.prf)
include(../include/util.prf)
#include(../include/propertybrowser.prf)
#include(../include/itempropetry.prf)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

CONFIG(debug, debug|release) {
    TARGET = graphicsitemsd
    DESTDIR = $$DLL_DIR_DEBUG
}
CONFIG(release, debug|release) {
    TARGET = graphicsitems
    DESTDIR = $$DLL_DIR_RELEASE
}

TEMPLATE = lib
DEFINES += GRAPHICSITEMS_LIBRARY

INCLUDEPATH +=  items\
                event

RESOURCES += \
    graphicsitems.qrc

HEADERS += \
    common/commondata.h \
    common/scene_global.h \
    common/treenode.h \
    items/baseitem.h \
    itemglobal.h \
    itemglobal.h \
    animation/animationstruct.h \
    animation/flashengine.h \
    graphicsview.h \
    graphicsscene.h \
    itemsfactory/itemsfactory.h \
    common/treedata.h \
    managescene.h \
    event/graphicsitmshandle.h \
    items/plugnodeitem.h \
    items/polylineitem.h \
    suspendwidget.h \
    items/regionselectitem.h \
    commands.h \
    commands.h \
    topsuspendwidget.h \
    itemsoperationsuspendwidget.h \
    itemsoperationtool.h


SOURCES += \
    common/commondata.cpp \
    common/treenode.cpp \
    items/baseitem.cpp \
    itemglobal.cpp \
    graphicsview.cpp \
    animation/flashengine.cpp \
    graphicsscene.cpp \
    itemsfactory/itemsfactory.cpp \
    common/treedata.cpp \
    managescene.cpp \
    event/graphicsitmshandle.cpp \
    items/plugnodeitem.cpp \
    items/polylineitem.cpp \
    suspendwidget.cpp \
    items/regionselectitem.cpp \
    commands.cpp \
    topsuspendwidget.cpp \
    itemsoperationsuspendwidget.cpp \
    itemsoperationtool.cpp


FORMS += \

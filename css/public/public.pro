#-------------------------------------------------
#
# Project created by QtCreator 2018-04-29T15:47:33
#
#-------------------------------------------------
include(../include/global.prf)
include(../include/util.prf)

QT       += widgets

TARGET = global
TEMPLATE = lib

CONFIG += C++11

DEFINES += COMMONMODULE_LIBRARY

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
    css_messagebox.cpp \
        multicombobox.cpp \
    pagetableview.cpp \
    checkboxdelegate.cpp \
    checkheaderview.cpp \
    topwindow.cpp \
    popupwindow.cpp \
    messagebox.cpp \
    popupwidget.cpp \
    popuptitle.cpp \
    calendartimewidget.cpp \
    timershaftwidget.cpp \
    customcalendarwidget.cpp \
    comboboxitemdelegate.cpp \
    tablemodel.cpp \
    object.cpp \
    treemodel.cpp \
    toolfiltertablemodel.cpp

HEADERS += \
        css_messagebox.h \
        multicombobox.h \
        commonmodule_global.h \ 
    pagetableview.h \
    checkboxdelegate.h \
    checkheaderview.h \
    topwindow.h \
    popupwindow.h \
    messagebox.h \
    popupwidget.h \
    popuptitle.h \
    calendartimewidget_P.h \
    timershaftwidget.h \
    customcalendarwidget.h \
    comboboxitemdelegate.h \
    tablemodel.h \
    iobjectnotifier.h \
    object.h \
    object_p.h \
    treemodel.h \
    toolfiltertablemodel.h

CONFIG(debug, debug|release) {
    TARGET = globald
    DESTDIR = $$DLL_DIR_DEBUG
}
CONFIG(release, debug|release) {
    TARGET = global
    DESTDIR = $$DLL_DIR_RELEASE
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    pagetableview.ui \
    topwindow.ui \
    popupwindow.ui \
    messagebox.ui \
    popuptitle.ui \
    calendartimewidget.ui \

RESOURCES += \
    commres.qrc


QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../include/global.prf)
include(../include/util.prf)
include(../include/public.prf)



SOURCES += \
    configuration/cpuinfodlg.cpp \
    configuration/frameworkinfodlg.cpp \
    configuration/inputeditor.cpp \
    configuration/inputinfowidget.cpp \
    configuration/moduleinfodlg.cpp \
    configuration/systeminfodlg.cpp \
    httpclient.cpp \
    main.cpp \
    mainwindow.cpp \
    scene/sceneeditorview.cpp


HEADERS += \
    configuration/configglobal.h \
    configuration/cpuinfodlg.h \
    configuration/frameworkinfodlg.h \
    configuration/inputeditor.h \
    configuration/inputinfowidget.h \
    configuration/moduleinfodlg.h \
    configuration/systeminfodlg.h \
    httpclient.h \
    mainwindow.h \
    scene/sceneeditorview.h

FORMS += \
    configuration/cpuinfodlg.ui \
    configuration/frameworkinfodlg.ui \
    configuration/inputeditor.ui \
    configuration/inputinfowidget.ui \
    configuration/moduleinfodlg.ui \
    configuration/systemInfoDlg.ui \
    mainwindow.ui \
    scene/sceneeditorview.ui




CONFIG(debug, debug|release) {
    TARGET = demo
    DESTDIR = $$DLL_DIR_DEBUG
}
CONFIG(release, debug|release) {
    TARGET = demo
    DESTDIR = $$DLL_DIR_RELEASE
}


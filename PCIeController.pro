#-------------------------------------------------
#
# Project created by QtCreator 2017-06-14T16:43:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PCIeController
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# INCLUDEPATH += $$PWD/winDriver/include
INCLUDEPATH += $$PWD/winDriver
INCLUDEPATH += $$PWD/winDriver/lib
INCLUDEPATH += $$PWD/winDriver/include
#
LIBS += $$PWD/winDriver/lib/wd_utils.dll
LIBS += $$PWD/winDriver/lib/wdlib.dll
#LIBS += -L$$PWD/winDriver/lib/-lwd_utils
#LIBS += -L$$PWD/winDriver/lib/-lwdlib

SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp \
    highspeedwindow.cpp \
    pciexpress.cpp

HEADERS  += mainwindow.h \
    mdichild.h \
    highspeedwindow.h \
    pciexpress.h

FORMS    += mainwindow.ui \
    highspeedwindow.ui

RESOURCES += \
    res.qrc

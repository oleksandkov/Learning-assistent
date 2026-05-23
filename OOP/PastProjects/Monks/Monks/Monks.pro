#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T23:25:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Monks
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainview.cpp \
    entity.cpp \
    newbie.cpp \
    expirienced.cpp \
    guru.cpp \
    stone1.cpp \
    stone2.cpp \
    stone3.cpp \
    interactionwithmonkdialog.cpp \
    interactionwithstonedialog.cpp \
    insertmonkdialog.cpp \
    minimap.cpp \
    chat.cpp

HEADERS  += mainwindow.h \
    mainview.h \
    entity.h \
    newbie.h \
    expirienced.h \
    guru.h \
    stone1.h \
    stone2.h \
    stone3.h \
    interactionwithmonkdialog.h \
    interactionwithstonedialog.h \
    insertmonkdialog.h \
    minimap.h \
    chat.h

FORMS    += mainwindow.ui \
    interactionwithmonkdialog.ui \
    interactionwithstonedialog.ui \
    insertmonkdialog.ui \
    minimap.ui \
    chat.ui

RESOURCES += \
    resources.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2016-08-18T11:22:38
#
#-------------------------------------------------

QT += core gui
QT += serialport

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EmbryoBot
TEMPLATE = app


SOURCES += main.cpp\
        frontpanel.cpp \
    pressurecontrol.cpp

HEADERS  += frontpanel.h \
    pressurecontrol.h

FORMS    += frontpanel.ui

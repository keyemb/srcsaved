#-------------------------------------------------
#
# Project created by QtCreator 2013-05-07T20:25:44
#
#-------------------------------------------------

QT       += core gui
QT       += sql

TARGET = zhiwen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    posix_qextserialport.cpp \
    form.cpp

HEADERS  += mainwindow.h \
    qextserialbase.h \
    posix_qextserialport.h \
    form.h

FORMS    += mainwindow.ui \
    form.ui

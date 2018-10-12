#-------------------------------------------------
#
# Project created by QtCreator 2016-08-03T10:47:00
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = collector
TEMPLATE = app

INCLUDEPATH+=D:\Work_Software\OpenCV3.1\opencv\build\include
LIBS+=D:\Work_Software\OpenCV3.1\opencv\build\x64\vc12\lib\*.lib

SOURCES += main.cpp\
        collector.cpp \
    processor.cpp \
    recorder.cpp \
    database.cpp \
    imagewidget.cpp

HEADERS  += collector.h \
    common.h \
    processor.h \
    recorder.h \
    database.h \
    imagewidget.h

FORMS    += collector.ui

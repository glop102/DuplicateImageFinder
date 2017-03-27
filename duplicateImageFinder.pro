#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T12:01:03
#
#-------------------------------------------------

QT				+= core gui
#QMAKE_CXXFLAGS	+= -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = duplicateImageFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagelibrary.cpp \
    imageinfo.cpp \
    scrollableimagedisplay.cpp \
    imagebutton.cpp

HEADERS  += mainwindow.h \
    imagelibrary.h \
    imageinfo.h \
    scrollableimagedisplay.h \
    imagebutton.h

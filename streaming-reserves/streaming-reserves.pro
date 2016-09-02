#***University of South Carolina - Music Library***
# Copyright (c) 2016 University of South Carolina
# Author: Adrian Quiroga
#
#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T17:57:11
#
#-------------------------------------------------

QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets\
    webkitwidgets

TARGET = streaming-reserves
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    audiofiledata.cpp \
    playlist.cpp \
    category.cpp \
    library.cpp \
    coursedetailsdialog.cpp \
    addpermalinkdialog.cpp \
    adddividerdialog.cpp \
    updater.cpp \
    tracksTableModel.cpp \
    previewdialog.cpp

HEADERS  += mainwindow.h \
    audiofiledata.h \
    utils.h \
    playlist.h \
    category.h \
    library.h \
    coursedetailsdialog.h \
    addpermalinkdialog.h \
    adddividerdialog.h \
    updater.h \
    tracksTableModel.h \
    previewdialog.h

FORMS    += mainwindow.ui \
    coursedetailsdialog.ui \
    addpermalinkdialog.ui \
    adddividerdialog.ui \
    previewdialog.ui

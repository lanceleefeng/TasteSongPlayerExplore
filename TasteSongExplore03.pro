#-------------------------------------------------
#
# Project created by QtCreator 2017-03-02T23:52:40
#
#-------------------------------------------------

QT       += core gui \
    network \
    xml \
    multimedia \
    multimediawidgets \
    sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TasteSongExplore03
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


SOURCES += main.cpp\
    player.cpp \
    videowidget.cpp \
    playercontrols.cpp \
    playlistmodel.cpp \
    db.cpp \
    musicmodel.cpp \
    dbinit.cpp \
    basemodel.cpp \
    settingmodel.cpp \
    tools.cpp \
    config.cpp

HEADERS  += \
    player.h \
    videowidget.h \
    playercontrols.h \
    playlistmodel.h \
    db.h \
    musicmodel.h \
    dbinit.h \
    basemodel.h \
    settingmodel.h \
    tools.h \
    config.h


RC_ICONS="TasteSong6.ico"

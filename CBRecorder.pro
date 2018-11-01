QT += core network widgets

CONFIG += c++11

TARGET = CBRecorder
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += $$PWD/ffmpeg/include

TEMPLATE = app

SOURCES += main.cpp \
    HttpRequest.cpp \
    HttpRequestWorker.cpp \
    Streamer.cpp \
    MainWindow.cpp \
    StreamerListView.cpp \
    StreamerListModel.cpp \
    StreamerListDelegate.cpp \
    StreamerManager.cpp \
    StreamerListItemForm.cpp \
    StreamRecorder.cpp \
    Utilities.cpp \
    NewStreamRecorder.cpp

HEADERS += \
    HttpRequest.h \
    HttpRequestWorker.h \
    Streamer.h \
    MainWindow.h \
    StreamerListView.h \
    StreamerListModel.h \
    StreamerListDelegate.h \
    StreamerManager.h \
    StreamerListItemForm.h \
    StreamRecorder.h \
    Utilities.h \
    NewStreamRecorder.h

LIBS +=

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

FORMS += \
    StreamerListItemForm.ui

RESOURCES += \
    images.qrc

DISTFILES +=

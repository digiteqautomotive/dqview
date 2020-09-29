TARGET = mgbview
TEMPLATE = app

QT      += core gui widgets av avwidgets
VERSION = 1.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\" \

SOURCES += src/main.cpp \
    src/application.cpp \
    src/gui.cpp \
    src/optionsdialog.cpp \
    src/dirselectwidget.cpp \
    src/streamtable.cpp \
    src/streamdialog.cpp \
    src/stream.cpp \
    src/camera.cpp \
    src/timer.cpp \
    src/videoplayer.cpp \
    src/camerainfo.cpp
HEADERS += src/application.h \
    src/gui.h \
    src/stream.h \
    src/optionsdialog.h \
    src/dirselectwidget.h \
    src/streamdialog.h \
    src/streaminfo.h \
    src/streamtable.h \
    src/camera.h \
    src/video.h \
    src/timer.h \
    src/videoplayer.h \
    src/camerainfo.h \
    src/options.h

RESOURCES += mgbview.qrc

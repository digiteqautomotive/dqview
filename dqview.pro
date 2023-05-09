TARGET = dqview
TEMPLATE = app

QT      += core gui widgets
VERSION = 3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\" \
  QT_NO_DEPRECATED_WARNINGS

SOURCES += src/main.cpp \
    src/device.cpp \
    src/application.cpp \
    src/deviceconfigdialog.cpp \
    src/deviceinfo.cpp \
    src/gui.cpp \
    src/logdialog.cpp \
    src/optionsdialog.cpp \
    src/dirselectwidget.cpp \
    src/streamtable.cpp \
    src/streamdialog.cpp \
    src/stream.cpp \
    src/camera.cpp \
    src/timer.cpp \
    src/videoplayer.cpp
HEADERS += src/application.h \
    src/device.h \
    src/deviceconfigdialog.h \
    src/deviceinfo.h \
    src/gui.h \
    src/log.h \
    src/logdialog.h \
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
    src/options.h

RESOURCES += dqview.qrc
LIBS += -lvlc
win32 {
    RC_ICONS = icons/app.ico
    LIBS += -lstrmiids -lole32
    IDL_FILES = fg4.idl
    idl_compiler.output = ${QMAKE_FILE_BASE}.h
    idl_compiler.commands = widl ${QMAKE_FILE_NAME}
    idl_compiler.input = IDL_FILES
    QMAKE_EXTRA_COMPILERS += idl_compiler
}

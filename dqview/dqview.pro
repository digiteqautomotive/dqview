TARGET = dqview
TEMPLATE = app

QT      += core gui widgets
VERSION = 4.3

isEmpty(BUILD) {
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
} else {
DEFINES += APP_VERSION=\\\"$${VERSION}.$${BUILD}\\\"
}
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

SOURCES += src/main.cpp \
    src/configcopydialog.cpp \
    src/device.cpp \
    src/application.cpp \
    src/deviceconfigdialog.cpp \
    src/deviceinfo.cpp \
    src/gui.cpp \
    src/logdialog.cpp \
    src/optionsdialog.cpp \
    src/dirselectwidget.cpp \
    src/screencapturedialog.cpp \
    src/streamtable.cpp \
    src/streamdialog.cpp \
    src/stream.cpp \
    src/camera.cpp \
    src/timer.cpp \
    src/videooutput.cpp \
    src/videoplayer.cpp
HEADERS += src/application.h \
    src/configcopydialog.h \
    src/device.h \
    src/deviceconfigdialog.h \
    src/deviceinfo.h \
    src/gui.h \
    src/log.h \
    src/logdialog.h \
    src/pixelformat.h \
    src/screencapture.h \
    src/screencapturedialog.h \
    src/stream.h \
    src/optionsdialog.h \
    src/dirselectwidget.h \
    src/streamdialog.h \
    src/streaminfo.h \
    src/streamtable.h \
    src/camera.h \
    src/video.h \
    src/timer.h \
    src/videofile.h \
    src/videooutput.h \
    src/videoplayer.h \
    src/options.h

RESOURCES += dqview.qrc
LIBS += -lvlc
win32 {
    QMAKE_CXXFLAGS += -Wno-overloaded-virtual

    HEADERS += src/framebuffer.h
    SOURCES += src/framebuffer.cpp

    INCLUDEPATH += ../baseclasses
    Release:LIBS += ../baseclasses/release/libstrmbase.a
    Debug:LIBS += ../baseclasses/debug/libstrmbase.a
    LIBS += -lstrmiids -lole32 -loleaut32 -luuid -lwinmm

    RC_ICONS = icons/app.ico

    IDL_FILES = fg4.idl
    idl_compiler.output = ${QMAKE_FILE_BASE}.h
    idl_compiler.commands = widl ${QMAKE_FILE_NAME}
    idl_compiler.input = IDL_FILES
    QMAKE_EXTRA_COMPILERS += idl_compiler
}

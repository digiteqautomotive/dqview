TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib
TARGET = baseclasses

QMAKE_CXXFLAGS += -Wno-parentheses -Wno-overloaded-virtual

HEADERS += amextra.h \
    checkbmi.h \
    ctlutil.h \
    dxmperf.h \
    msgthrd.h \
    perflog.h \
    pullpin.h \
    renbase.h \
    source.h \
    sysclock.h \
    videoctl.h \
    winutil.h \
    wxutil.h \
    amfilter.h \
    combase.h \
    ddmm.h \
    fourcc.h \
    mtype.h \
    perfstruct.h \
    refclock.h \
    schedule.h \
    streams.h \
    transfrm.h \
    vtrans.h \
    wxdebug.h \
    cache.h \
    cprop.h \
    dllsetup.h \
    measure.h \
    outputq.h \
    pstream.h \
    reftime.h \
    seekpt.h \
    strmctl.h \
    transip.h \
    winctrl.h \
    wxlist.h

SOURCES += amextra.cpp \
    arithutil.cpp \
    ctlutil.cpp \
    dllsetup.cpp \
    perflog.cpp \
    refclock.cpp \
    seekpt.cpp \
    sysclock.cpp \
    videoctl.cpp \
    winutil.cpp \
    wxutil.cpp \
    amfilter.cpp \
    combase.cpp \
    ddmm.cpp \
    mtype.cpp \
    pstream.cpp \
    renbase.cpp \
    source.cpp \
    transfrm.cpp \
    vtrans.cpp \
    wxdebug.cpp \
    amvideo.cpp \
    cprop.cpp \
    dllentry.cpp \
    outputq.cpp \
    pullpin.cpp \
    schedule.cpp \
    strmctl.cpp \
    transip.cpp \
    winctrl.cpp \
    wxlist.cpp

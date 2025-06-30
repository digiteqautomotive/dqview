TEMPLATE = subdirs

SUBDIRS = app
app.subdir = dqview

win32 {
    SUBDIRS += baseclasses

    baseclasses.subdir = baseclasses
    app.depends = baseclasses
}

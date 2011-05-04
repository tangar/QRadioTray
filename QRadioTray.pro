#-------------------------------------------------
#
# Project created by QtCreator 2011-04-18T23:32:33
#
#-------------------------------------------------

QT       += core \
    network \
    phonon

TARGET  = QRadioTray

CONFIG += release

CONFIG(release, debug|release) {
#release options
} else {
#debug  options
CONFIG   += console
}

TEMPLATE = app
INCLUDEPATH += . \
    src

UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp

TRANSLATIONS    = qradiotray_en.ts \
                   qradiotray_ru.ts

CODECFORSRC     = UTF-8

SOURCES += src/main.cpp \
    src/application.cpp \
    src/settingsdialog.cpp \
    src/appenddialog.cpp \
    src/MyPlayer.cpp \
    src/aboutdialog.cpp \
    src/streamwrapper.cpp

HEADERS += \
    src/application.h \
    src/settingsdialog.h \
    src/appenddialog.h \
    src/Station.h \
    src/MyPlayer.h \
    src/aboutdialog.h \
    src/streamwrapper.h

FORMS += \
    ui/settingsdialog.ui \
    ui/appenddialog.ui \
    ui/aboutdialog.ui

RESOURCES += \
    resources.qrc

win32:RC_FILE = QRadioTray.rc

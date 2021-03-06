TEMPLATE = app
TARGET = qradiotray
DEPENDPATH += . debug release src ui translations
INCLUDEPATH += . src
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp

QT += core network phonon
CONFIG += qxt
QXT += core gui

unix {
    target.path = /usr/bin
    INSTALLS += target
    data.path = /usr/share/qradiotray
    data.files += config.ini
    INSTALLS += data
    DEFINES += UNIX_BUILD
}

# CONFIG += debug_and_release build_all
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    linux-g++: OBJECTS_DIR = debug
    DESTDIR = debug
    CONFIG += console
    QMAKE_CXXFLAGS_DEBUG += -pg
    QMAKE_LFLAGS_DEBUG += -pg
}
else {
    DEFINES += QT_NO_DEBUG_OUTPUT
    linux-g++: OBJECTS_DIR = release
    DESTDIR = release
}

TRANSLATIONS += \
    qradiotray_en.ts \
    qradiotray_ru.ts

SOURCES += \
    main.cpp \
    application.cpp \
    player.cpp \
    settingsdialog.cpp \
    stationdialog.cpp \
    aboutdialog.cpp \
    logger.cpp

HEADERS += \
    application.h \
    player.h \
    station.h \
    settingsdialog.h \
    stationdialog.h \
    aboutdialog.h \
    logger.h

FORMS += \
    settingsdialog.ui \
    stationdialog.ui \
    aboutdialog.ui

RESOURCES += resources.qrc
win32:RC_FILE = qradiotray.rc

OTHER_FILES += \
    README \
    Changelog.txt \
    config.ini \
    CODING_STYLE_ru.txt \
    CODING_STYLE_en.txt

# Proyecto Qt sencillo: QMake compila directamente la aplicación.

TEMPLATE = app
QT += core gui widgets multimedia
TARGET = AudioInspectorQt
CONFIG -= console debug_and_release debug_and_release_target debug
CONFIG += release

SOURCES += \
    src/main.cpp \
    src/audioanalyzer.cpp \
    src/dspanalysis.cpp \
    src/localization.cpp \
    src/waveformwidget.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/audioanalyzer.h \
    src/dspanalysis.h \
    src/localization.h \
    src/waveformwidget.h \
    src/mainwindow.h

FORMS += \
    forms/mainwindow.ui

OTHER_FILES += \
    README.md \
    LICENSE \
    TRADEMARKS.md \
    scripts/package-portable.ps1 \
    .gitignore

DEFINES += AUDIOINSPECTOR_VERSION=0.1.0

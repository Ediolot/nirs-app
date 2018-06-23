#-------------------------------------------------
#
# Project created by QtCreator 2018-05-19T16:00:03
#
#-------------------------------------------------

QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NIRS_APP
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "D:\Libs"
INCLUDEPATH += "D:\Libs\DSPFiltersComplete\shared\DSPFilters\include"
INCLUDEPATH += "C:\Deps"
INCLUDEPATH += "C:\Deps\DSPFiltersComplete\shared\DSPFilters\include"

SOURCES += \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Bessel.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Biquad.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Butterworth.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Cascade.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\ChebyshevI.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\ChebyshevII.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Custom.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Design.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Documentation.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Elliptic.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Filter.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Legendre.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\Param.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\PoleFilter.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\RBJ.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\RootFinder.cpp" \
    "D:\Libs\DSPFiltersComplete\shared\DSPFilters\source\State.cpp"

SOURCES += \
    src/experiment.cpp \
    src/frame.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/webengine.cpp \
    src/webinterface.cpp \
    src/exceptions/filenotfoundexception.cpp \
    src/exceptions/filereaderrorexception.cpp \
    src/exceptions/framebpptoobig.cpp \
    src/tasklauncher.cpp \
    src/task.cpp \
    src/signal.cpp

HEADERS +=\
    src/experiment.h \
    src/frame.h \
    src/mainwindow.h \
    src/webengine.h \
    src/webinterface.h \
    src/exceptions/filenotfoundexception.h \
    src/exceptions/filereaderrorexception.h \
    src/exceptions/framebpptoobig.h \
    src/tasklauncher.h \
    src/task.h \
    src/signal.h

FORMS += \
    forms/mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources/resources.qrc

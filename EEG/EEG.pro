# Makefile of the library "EEG"
# Paul Boersma, 8 August 2017
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../kar ../sys ../dwsys ../stat ../dwtools ../fon

SOURCES += \
    EEG.cpp \
    EEGWindow.cpp \
    ERP.cpp \
    ERPTier.cpp \
    ERPWindow.cpp \
    manual_EEG.cpp \
    praat_EEG.cpp

HEADERS += \
    EEG.h \
    EEG_def.h \
    EEGWindow.h \
    EEGWindow_prefs.h \
    ERP.h \
    ERP_def.h \
    ERPTier.h \
    ERPTier_def.h \
    ERPWindow.h \
    ERPWindow_prefs.h

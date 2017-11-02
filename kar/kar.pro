# Makefile of the library "kar"
# Paul Boersma, 5 October 2017
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../sys

SOURCES += \
    ipaSerifRegularPS.cpp \
    longchar.cpp

HEADERS += \
    longchar.h \
    UnicodeData.h


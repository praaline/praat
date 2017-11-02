# Makefile of the library "num"
# Paul Boersma, 8 August 2017
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../external/gsl ../external/glpk ../sys ../dwsys

SOURCES += \
    NUM.cpp \
    NUMarrays.cpp \
    NUMear.cpp \
    NUMlinprog.cpp \
    NUMrandom.cpp \
    NUMsort.cpp


# Makefile of the library "FFNet"
# David Weenink and Paul Boersma, 8 August 2017
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../kar ../sys ../dwtools ../fon ../dwsys ../stat ../gram

SOURCES += \
    FFNet.cpp \
    FFNet_Eigen.cpp FFNet_Matrix.cpp FFNet_PatternList.cpp \
    FFNet_ActivationList_Categories.cpp FFNet_PatternList_ActivationList.cpp \
    FFNet_PatternList_Categories.cpp RBM_extensions.cpp \
    praat_FFNet_init.cpp manual_FFNet.cpp

HEADERS += \
    FFNet.h \
    FFNet_ActivationList_Categories.h \
    FFNet_def.h \
    FFNet_Eigen.h \
    FFNet_Matrix.h \
    FFNet_PatternList.h \
    FFNet_PatternList_ActivationList.h \
    FFNet_PatternList_Categories.h \
    praat_FFNet.h \
    RBM_extensions.h

# Makefile of the library "stat"
# Paul Boersma, 8 August 2017
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../kar ../sys ../dwsys ../dwtools ../fon ../stat

SOURCES += \
    Table.cpp TableEditor.cpp Regression.cpp manual_statistics.cpp \
    praat_TableOfReal.cpp praat_Stat.cpp \
    TableOfReal.cpp Distributions.cpp Distributions_and_Strings.cpp PairDistribution.cpp \
    LogisticRegression.cpp

HEADERS += \
    Distributions.h \
    Distributions_and_Strings.h \
    LogisticRegression.h \
    LogisticRegression_def.h \
    PairDistribution.h \
    PairDistribution_def.h \
    praat_TableOfReal.h \
    Regression.h \
    Regression_def.h \
    Table.h \
    Table_def.h \
    TableEditor.h \
    TableEditor_prefs.h \
    TableOfReal.h \
    TableOfReal_def.h

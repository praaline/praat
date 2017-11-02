# Makefile of the library "contrib/ola"
# Ola So"der 19 January 2008
# Paul Boersma, 2 June 2017
# QMake version George Christodoulides, October 2017

! include( ../../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../../kar ../../sys ../../FFNet ../../dwtools ../../fon ../../dwsys ../../stat ../../external/gsl
DEFINES += _DEBUG
DEFINES += _REENTRANT

SOURCES += \
    KNN.cpp KNN_threads.cpp Pattern_to_Categories_cluster.cpp \
    KNN_prune.cpp FeatureWeights.cpp praat_contrib_Ola_KNN.cpp manual_KNN.cpp

HEADERS += \
    FeatureWeights.h \
    FeatureWeights_def.h \
    KNN.h \
    KNN_def.h \
    KNN_prune.h \
    KNN_threads.h \
    OlaP.h \
    Pattern_to_Categories_cluster.h

DISTFILES += \
    BUGS.txt \
    ChangeLog.txt \
    TODO.txt

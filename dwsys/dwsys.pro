# makefile for library "dwsys".
# David Weenink 20171019
# Paul Boersma 2017-08-08
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../stat ../sys ../external/gsl ../kar

SOURCES += \
    Collection_extensions.cpp Command.cpp \
    DoublyLinkedList.cpp Eigen.cpp \
    FileInMemory.cpp FileInMemorySet.cpp FileInMemoryManager.cpp\
    Graphics_extensions.cpp Index.cpp \
    NUM2.cpp NUMhuber.cpp NUMlapack.cpp NUMmachar.cpp \
    NUMf2c.cpp NUMcblas.cpp NUMclapack.cpp NUMcomplex.cpp NUMfft_d.cpp NUMsort2.cpp \
    NUMmathlib.cpp NUMstring.cpp \
    Permutation.cpp Permutation_and_Index.cpp \
    regularExp.cpp SimpleVector.cpp Simple_extensions.cpp \
    SVD.cpp

HEADERS += \
    Collection_extensions.h \
    Command.h \
    DoublyLinkedList.h \
    Eigen.h \
    Eigen_def.h \
    FileInMemory.h \
    FileInMemory_def.h \
    FileInMemoryManager.h \
    FileInMemoryManager_def.h \
    FileInMemorySet.h \
    FileInMemorySet_def.h \
    Graphics_extensions.h \
    Index.h \
    Index_def.h \
    NUM2.h \
    NUMcblas.h \
    NUMclapack.h \
    NUMcomplex.h \
    NUMf2c.h \
    NUMfft_core.h \
    NUMlapack.h \
    NUMmachar.h \
    Permutation.h \
    Permutation_and_Index.h \
    Permutation_def.h \
    regularExp.h \
    Simple_extensions.h \
    SimpleVector.h \
    SimpleVector_def.h \
    SVD.h \
    SVD_def.h


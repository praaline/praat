# Makefile of the library "external/mp3"
# Erez Volk, 24 May 2007
# Paul Boersma, 24 August 2013
# QMake version George Christodoulides, September 2017

! include( ../../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../../sys

SOURCES += \
    mp3.cpp \
    mad_bit.c \
    mad_decoder.c \
    mad_fixed.c \
    mad_frame.c \
    mad_huffman.c \
    mad_layer12.c \
    mad_layer3.c \
    mad_stream.c \
    mad_synth.c \
    mad_timer.c \
    mad_version.c

DISTFILES += \
    mad_D.dat \
    mad_imdct_s.dat \
    mad_qc_table.dat \
    mad_rq_table.dat \
    mad_sf_table.dat \

HEADERS += \
    mad_bit.h \
    mad_config.h \
    mad_decoder.h \
    mad_fixed.h \
    mad_frame.h \
    mad_global.h \
    mad_huffman.h \
    mad_layer3.h \
    mad_layer12.h \
    mad_stream.h \
    mad_synth.h \
    mad_timer.h \
    mad_version.h \
    mp3.h

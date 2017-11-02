# makefile for libespeak
# QMake version George Christodoulides, September 2017

! include( ../../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += . ../../kar ../../sys ../../dwsys ../../stat

SOURCES += \
    categories.cpp compiledata.cpp compiledict.cpp dictionary.cpp encoding.cpp error.cpp intonation.cpp klatt.cpp  \
    numbers.cpp phonemelist.cpp proplist.cpp readclause.cpp setlengths.cpp speech.cpp \
    synthdata.cpp synthesize.cpp \
    synth_mbrola.cpp tr_languages.cpp mnemonics.cpp translate.cpp voices.cpp wavegen.cpp \
    create_espeak_ng_FileInMemoryManager.cpp create_espeak_ng_FileInMemorySet.cpp \
    espeakdata_FileInMemory.cpp espeak_api.cpp espeak_io.cpp \

HEADERS += \
    config.h \
    encoding.h \
    error.h \
    espeak_command.h \
    espeakdata_FileInMemory.h \
    espeak_io.h \
    event.h \
    fifo.h \
    klatt.h \
    phoneme.h \
    sintab.h \
    speak_lib.h \
    spect.h \
    speech.h \
    StdAfx.h \
    synthesize.h \
    translate.h \
    ucd.h \
    voice.h

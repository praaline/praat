# Qt version of Praat
# George Christodoulides, 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

# Will build the final executable in the main project directory.
TARGET = ../Praat
!mac*:TARGET = ../praat

TEMPLATE = app

CONFIG += thread warn_on stl rtti exceptions c++11
QT -= core gui

# Build folder organisation
CONFIG( debug, debug|release ) {
    # debug
    COMPONENTSPATH = build/debug
} else {
    # release
    COMPONENTSPATH = build/release
}

CONFIG += no_lflags_merge

# Application components
LIBS +=  \
    ../fon/$${COMPONENTSPATH}/libfon.a \
    ../contrib/ola/$${COMPONENTSPATH}/libola.a \
    ../artsynth/$${COMPONENTSPATH}/libartsynth.a \
    ../FFNet/$${COMPONENTSPATH}/libFFNet.a \
    ../gram/$${COMPONENTSPATH}/libgram.a \
    ../EEG/$${COMPONENTSPATH}/libEEG.a \
    ../LPC/$${COMPONENTSPATH}/libLPC.a \
    ../dwtools/$${COMPONENTSPATH}/libdwtools.a \
    ../fon/$${COMPONENTSPATH}/libfon.a \
    ../stat/$${COMPONENTSPATH}/libstat.a \
    ../dwsys/$${COMPONENTSPATH}/libdwsys.a \
    ../sys/$${COMPONENTSPATH}/libsys.a \
    ../num/$${COMPONENTSPATH}/libnum.a \
    ../kar/$${COMPONENTSPATH}/libkar.a \
    ../external/espeak/$${COMPONENTSPATH}/libespeak.a \
    ../external/portaudio/$${COMPONENTSPATH}/libportaudio.a \
    ../external/flac/$${COMPONENTSPATH}/libflac.a \
    ../external/mp3/$${COMPONENTSPATH}/libmp3.a \
    ../external/glpk/$${COMPONENTSPATH}/libglpk.a \
    ../external/gsl/$${COMPONENTSPATH}/libgsl.a \
    $$LIBS

PRE_TARGETDEPS += \
    ../external/gsl/$${COMPONENTSPATH}/libgsl.a \
    ../external/glpk/$${COMPONENTSPATH}/libglpk.a \
    ../external/mp3/$${COMPONENTSPATH}/libmp3.a \
    ../external/flac/$${COMPONENTSPATH}/libflac.a \
    ../external/portaudio/$${COMPONENTSPATH}/libportaudio.a \
    ../external/espeak/$${COMPONENTSPATH}/libespeak.a \
    ../kar/$${COMPONENTSPATH}/libkar.a \
    ../num/$${COMPONENTSPATH}/libnum.a \
    ../sys/$${COMPONENTSPATH}/libsys.a \
    ../dwsys/$${COMPONENTSPATH}/libdwsys.a \
    ../stat/$${COMPONENTSPATH}/libstat.a \
    ../fon/$${COMPONENTSPATH}/libfon.a \
    ../dwtools/$${COMPONENTSPATH}/libdwtools.a \
    ../LPC/$${COMPONENTSPATH}/libLPC.a \
    ../EEG/$${COMPONENTSPATH}/libEEG.a \
    ../gram/$${COMPONENTSPATH}/libgram.a \
    ../FFNet/$${COMPONENTSPATH}/libFFNet.a \
    ../artsynth/$${COMPONENTSPATH}/libartsynth.a \
    ../contrib/ola/$${COMPONENTSPATH}/libola.a

INCLUDEPATH += ../kar ../sys ../fon

SOURCES += \
    main_Praat.cpp


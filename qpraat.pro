# Qt version of Praat
# George Christodoulides, 2017

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += external/gsl
SUBDIRS += external/glpk
SUBDIRS += external/mp3
SUBDIRS += external/flac
SUBDIRS += external/portaudio
SUBDIRS += external/espeak

SUBDIRS += kar
SUBDIRS += num
SUBDIRS += sys
SUBDIRS += dwsys
SUBDIRS += stat
SUBDIRS += fon
SUBDIRS += dwtools

SUBDIRS += LPC
SUBDIRS += EEG
SUBDIRS += gram
SUBDIRS += FFNet
SUBDIRS += artsynth
SUBDIRS += contrib/ola
SUBDIRS += main

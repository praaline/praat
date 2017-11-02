# Makefile of the library "gram"
# Paul Boersma 2017-10-16
# QMake version George Christodoulides, October 2017

! include( ../common.pri ) {
    error( Could not find the common.pri file! )
}

TEMPLATE = lib

CONFIG += staticlib thread warn_on stl rtti exceptions c++11
QT -= core gui

INCLUDEPATH += ../kar ../sys ../dwsys ../stat ../dwtools ../fon

SOURCES += \
    Network.cpp \
    OTGrammar.cpp OTGrammarEditor.cpp manual_gram.cpp praat_gram.cpp OTMulti.cpp OTMultiEditor.cpp \
    OTGrammar_ex_metrics.cpp OTGrammar_ex_NoCoda.cpp OTGrammar_ex_NPA.cpp OTGrammar_ex_tongueRoot.cpp \
    OTMulti_ex_metrics.cpp RBM.cpp DeepBeliefNetwork.cpp

HEADERS += \
    DeepBeliefNetwork.h \
    DeepBeliefNetwork_def.h \
    DeepBeliefNetwork_enums.h \
    Network.h \
    Network_def.h \
    Network_enums.h \
    OTGrammar.h \
    OTGrammar_def.h \
    OTGrammarEditor.h \
    OTGrammar_enums.h \
    OTMulti.h \
    OTMulti_def.h \
    OTMultiEditor.h \
    RBM.h \
    RBM_def.h

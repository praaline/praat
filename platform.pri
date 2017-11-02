# Platform file for linux.pulse
# David Weenink and Paul Boersma, 22 October 2016
# QMake version George Christodoulides, October 2017

DEFINES += UNIX
DEFINES += linux
DEFINES += ALSA
DEFINES += HAVE_PULSEAUDIO
DEFINES += _FILE_OFFSET_BITS=64

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0
}

# -Werror=missing-prototypes -Werror=implicit -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized
# -O3 -g1 -pthread

LIBS += -lm -lpulse -lasound -lpthread

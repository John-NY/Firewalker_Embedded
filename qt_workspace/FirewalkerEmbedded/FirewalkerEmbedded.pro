#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T23:14:56
#
#-------------------------------------------------

QT       += network core serialport

QT       -= gui

TARGET = FirewalkerEmbedded
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#PUREGEV_ROOT        ?= /opt/pleora/ebus_sdk/Ubuntu-12.04-x86_64
#PV_LIBRARY_PATH      =$(PUREGEV_ROOT)/lib

#CFLAGS              += -I$(PUREGEV_ROOT)/include -I.
#CPPFLAGS            += -I$(PUREGEV_ROOT)/include -I.

#CFLAGS    += -D_UNIX_ -D_LINUX_
#CPPFLAGS  += -D_UNIX_ -D_LINUX_


#LDFLAGS             += -L$(PUREGEV_ROOT)/lib         \
#                        -lPvBase                     \
#                        -lPvDevice                   \
#                        -lPvBuffer                   \
#                        -lPvPersistence              \
#                        -lPvGenICam                  \
#                        -lPvStream                   \
#                        -lPvTransmitter              \
#                        -lPvVirtualDevice	         \
#                                    -lPvAppUtils                 \
#                        -lPvSerial

#LDFLAGS   += -lPvGUI
#LDFLAGS   += -lSimpleImagingLib
#GEN_LIB_PATH = $(PUREGEV_ROOT)/lib/genicam/bin/Linux64_x64
#LDFLAGS      += -L$(GEN_LIB_PATH)
#FILES_QTGUI          = $(shell grep -l QtGui *)

SOURCES += main.cpp \
    masterclock.cpp \
    udppacketsender.cpp \
    udppacketlistener.cpp\
    megaserialclass.cpp \
    telemetryclass.cpp \
    filewriterclass.cpp

HEADERS += \
    masterclock.h \
    udppacketsender.h \
    udppacketlistener.h \
    megaserialclass.h \
    telemetryclass.h \
    filewriterclass.h

#-------------------------------------------------
#
# Project created by QtCreator 2019-06-26T11:47:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network charts

TARGET = Oscilloscope
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    simplescope.cpp \
    globalchannellistview.cpp \
    localchannellistview.cpp \
    channellist.cpp \
    globalchannellist.cpp \
    ilistview.cpp \
    localchannellist.cpp \
    attributes.cpp \
    ichannel.cpp \
    datastream.cpp \
    channel.cpp \
    dublicatechannel.cpp \
    channelcontroller.cpp \
    server.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    frameparser.cpp \
    kiss_fft.cpp \
    signalsmoothing.cpp \
    display.cpp \
    trigger.cpp \
    udpserver.cpp \
    serversettings.cpp \
    marker.cpp \
    markermeasurements.cpp \
    recorder.cpp \
    recordframeparser.cpp \
    playrecordspeeddialog.cpp

HEADERS += \
        mainwindow.h \
    simplescope.h \
    globalchannellistview.h \
    localchannellistview.h \
    globalchannellist.h \
    channellist.h \
    ilistview.h \
    localchannellist.h \
    attributes.h \
    ichannel.h \
    datastream.h \
    frame.h \
    channel.h \
    dublicatechannel.h \
    channelcontroller.h \
    server.h \
    tcpserver.h \
    tcpclient.h \
    frameparser.h \
    kiss_fft.h \
    signalsmoothing.h \
    enums.h \
    display.h \
    trigger.h \
    udpserver.h \
    serversettings.h \
    marker.h \
    markermeasurements.h \
    recorder.h \
    recordframeparser.h \
    playrecordspeeddialog.h

FORMS += \
        mainwindow.ui \
    simplescope.ui

RESOURCES += \
    icons.qrc

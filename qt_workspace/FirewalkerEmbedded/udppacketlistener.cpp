/******************************************************************************
 * UDP Packet Listener
 *
 * This monitors the LISTENER_PORT for incoming commands from the base station.
 * Typical commands from the base station include:
 *  - Notification of new waypoints to ingest (include name of file)
 *  - Notification of new Arduino Mega bitfiles to load to the sensor module
 *      (include name of file)
 *  - Request for 5Hz streaming telemetry data.
 *  - Commands from Ardupilot software to MAVLINK
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#include "udppacketlistener.h"

udpPacketListener::udpPacketListener(QObject *parent) : QObject(parent)
{
//! [0]
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(LISTENER_PORT, QUdpSocket::ShareAddress);
//! [0]

//! [1]
    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));
//! [1]
}

udpPacketListener::~udpPacketListener()
{

}

void udpPacketListener::processPendingDatagrams()
{
//! [2]
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        fprintf(stdout,".");
        fflush(stdout);
    }
//! [2]
}


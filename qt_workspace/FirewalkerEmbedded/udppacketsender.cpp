/******************************************************************************
 * UDP Packet Sender
 *
 * This broadcasts telemetry data and notifications.
 * Typical notifications may include:
 *  - Streaming telemetry (e.g. GPS / Temperature / Sensor data)
 *  - Acknowledgement of received commands / completed operations
 *      - E.g. commanded update of Mega bitfile
 *      - E.g. Update to waypoints
 *  - Emergency conditions, if monitored (e.g. overtemp/battery warnings)
 *  - Notification of a new file / file finished notification
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#include "udppacketsender.h"

udpPacketSender::udpPacketSender(QObject *parent) : QObject(parent)
{
//! [0]
    udpSocket = new QUdpSocket(this);
//! [0]
    messageNo = 1;
}

udpPacketSender::~udpPacketSender()
{

}

void udpPacketSender::sendDatagram(QByteArray datagram)
{
    fprintf(stdout,"Trying to send data: ");
    fprintf(stdout,"%s\n",datagram.data());
    fflush(stdout);
//! [1]
//    QByteArray datagram = "Broadcast message " + QByteArray::number(messageNo);
    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress::Broadcast, SENDER_PORT);
//! [1]
    ++messageNo;
}


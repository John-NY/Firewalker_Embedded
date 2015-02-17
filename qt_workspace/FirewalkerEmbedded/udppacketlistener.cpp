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


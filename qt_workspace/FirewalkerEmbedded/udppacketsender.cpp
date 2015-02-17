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

void udpPacketSender::sendDatagram()
{
//! [1]
    QByteArray datagram = "Broadcast message " + QByteArray::number(messageNo);
    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress::Broadcast, SENDER_PORT);
//! [1]
    ++messageNo;
}


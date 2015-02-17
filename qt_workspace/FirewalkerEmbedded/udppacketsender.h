#ifndef UDPPACKETSENDER_H
#define UDPPACKETSENDER_H

#include <QObject>
#include <QUdpSocket>

#define SENDER_PORT 45454

class udpPacketSender : public QObject
{
    Q_OBJECT
public:
    explicit udpPacketSender(QObject *parent = 0);
    ~udpPacketSender();

private:
    QUdpSocket* udpSocket;
    int messageNo;

signals:

public slots:
    void sendDatagram();

};

#endif // UDPPACKETSENDER_H

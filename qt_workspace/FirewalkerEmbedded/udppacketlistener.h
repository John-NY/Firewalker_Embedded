#ifndef UDPPACKETLISTENER_H
#define UDPPACKETLISTENER_H

#include <QObject>
#include <QUdpSocket>
#include <stdio.h>

#define LISTENER_PORT 45454

class udpPacketListener : public QObject
{
    Q_OBJECT
public:
    explicit udpPacketListener(QObject *parent = 0);
    ~udpPacketListener();

private:
    QUdpSocket* udpSocket;

signals:

public slots:
    void processPendingDatagrams();

};

#endif // UDPPACKETLISTENER_H

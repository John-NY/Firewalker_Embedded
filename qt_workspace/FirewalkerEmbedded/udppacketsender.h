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

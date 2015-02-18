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

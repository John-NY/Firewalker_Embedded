/******************************************************************************
 * Telemetry Class
 *
 * This class holds telemetry data from the sensor module and Ardupilot that
 * is not written directly to file.
 * [ Telemetry Class will be reduced or deleted since telemetry is to be
 * written to filesystem for external processing. ]
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#ifndef TELEMETRYCLASS_H
#define TELEMETRYCLASS_H

#include <QObject>
#include <stdio.h>

#define N_LAVA 7

class telemetryClass : public QObject
{
    Q_OBJECT
public:
    explicit telemetryClass(QObject *parent = 0);
    ~telemetryClass();

private:
    int lavaSenseFov[N_LAVA];
    int lavaSenseDie[N_LAVA];

signals:
    void reportTelemetryUpdate(QByteArray);

public slots:
    int parseTelemetryPacket(QByteArray);
};

#endif // TELEMETRYCLASS_H

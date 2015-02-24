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
#include "telemetryclass.h"

telemetryClass::telemetryClass(QObject *parent) : QObject(parent)
{

}

telemetryClass::~telemetryClass()
{

}

int telemetryClass::parseTelemetryPacket(QByteArray myData )
{
    QString myTelemetry(myData);
    fprintf(stdout,"Received: %s\n",myTelemetry.toLocal8Bit().data());
    fflush(stdout);
    emit reportTelemetryUpdate(myData);
    return 0;
}


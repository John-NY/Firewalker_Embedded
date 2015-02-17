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
    return 0;
}


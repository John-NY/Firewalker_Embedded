#include <QCoreApplication>
#include <QThread>
#include "masterclock.h"
#include "udppacketlistener.h"
#include "udppacketsender.h"
#include "megaserialclass.h"
#include "telemetryclass.h"
#include "filewriterclass.h"

void configureSerial(QSerialPort* serialPort)
{
    serialPort->setPortName("/dev/ttyUSB1");
    serialPort->setBaudRate(QSerialPort::Baud57600);
    serialPort->open(QIODevice::ReadWrite);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread masterClockThread;
    QThread udpSenderThread;
    QThread udpListenerThread;
    QThread megaSerialThread;
    QThread fileWriterThread;

    QSerialPort* mySerial = new QSerialPort();
    configureSerial(mySerial);

    MasterScheduler* masterClock = new MasterScheduler();
    udpPacketListener* myListener = new udpPacketListener();
    udpPacketSender* mySender = new udpPacketSender();
    megaSerialClass* myMega = new megaSerialClass(mySerial);
    telemetryClass* myTelemetry = new telemetryClass();
    fileWriterClass* myTelemetryFile = new fileWriterClass();

    masterClock->moveToThread(&masterClockThread);
    mySender->moveToThread(&udpSenderThread);
    myListener->moveToThread(&udpListenerThread);
    myMega->moveToThread(&megaSerialThread);
    mySerial->moveToThread(&megaSerialThread);
    myTelemetry->moveToThread(&udpSenderThread);
    myTelemetryFile->moveToThread(&udpSenderThread);

    QString filename = "megaTelemetry";
    myTelemetryFile->createFile(filename);

    /** Connect Actions to Master Clock **/
//    QObject::connect()
    QObject::connect(masterClock,SIGNAL(doAction1Hz()),myMega,SLOT(requestTelemetry()));
    QObject::connect(masterClock,SIGNAL(doAction5Hz()),mySender,SLOT(sendDatagram()));

    /** Connect Other Actions **/
//    QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),myTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
    QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),myTelemetryFile,SLOT(writeToFile(QByteArray)));

    /** Clean Up Classes on Thread Close **/
    QObject::connect(&masterClockThread,SIGNAL(finished()),masterClock,SLOT(deleteLater()));
    QObject::connect(&udpSenderThread,SIGNAL(finished()),mySender,SLOT(deleteLater()));
    QObject::connect(&udpListenerThread,SIGNAL(finished()),myListener,SLOT(deleteLater()));
    QObject::connect(&udpListenerThread,SIGNAL(finished()),myTelemetry,SLOT(deleteLater()));
    QObject::connect(&megaSerialThread,SIGNAL(finished()),myMega,SLOT(deleteLater()));
    QObject::connect(&megaSerialThread,SIGNAL(finished()),mySerial,SLOT(deleteLater()));

    masterClockThread.start();
    udpListenerThread.start();
    udpSenderThread.start();
    megaSerialThread.start();
    fileWriterThread.start();

    /* Set thread priority after starting thread */
    masterClockThread.setPriority(QThread::HighestPriority);

    masterClock->doWork();

    masterClockThread.wait();

    return a.exec();
}

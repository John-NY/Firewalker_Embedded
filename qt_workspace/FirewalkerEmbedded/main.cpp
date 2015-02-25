#include <QCoreApplication>
#include <QThread>
#include "masterclock.h"
#include "udppacketlistener.h"
#include "udppacketsender.h"
#include "megaserialclass.h"
#include "powerswitcharduino.h"
#include "telemetryclass.h"
#include "filewriterclass.h"

#define SENSOR_SERIAL_MEGA "/dev/ttyUSB0"
#define POWER_SERIAL_ARDU "/dev/ttyUSB1"

void configureSerial(QSerialPort* serialPort, QString sPort)
{
    serialPort->setPortName(sPort);
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
    QThread powerSerialThread;
    QThread fileWriterThread;


    MasterScheduler* masterClock = new MasterScheduler();
    masterClock->moveToThread(&masterClockThread); // Needs a thread to execute


    /** Set up the various pieces which handle remote communications **/
    udpPacketListener* myListener = new udpPacketListener();
    udpPacketSender* mySender = new udpPacketSender();
    mySender->moveToThread(&udpSenderThread); // Needs a thread to execute
    myListener->moveToThread(&udpListenerThread); // Needs a thread to execute

    /** Set up the various pieces that handle and log the sensor readings and telemetry **/
    QSerialPort* megaSerial = new QSerialPort(); // serial port for comms
    configureSerial(megaSerial,SENSOR_SERIAL_MEGA); // configured standard
    megaSerialClass* myMega = new megaSerialClass(megaSerial); // handles send/receive/signals
    telemetryClass* megaTelemetry = new telemetryClass(); // Sensor Telemetry Holder
    fileWriterClass* megaTelemetryFile = new fileWriterClass(); // Sensor Telemetry Logger
    myMega->moveToThread(&megaSerialThread); // Needs a thread to execute
    megaSerial->moveToThread(&megaSerialThread); // Needs a thread to execute
    megaTelemetry->moveToThread(&udpSenderThread); // Needs a thread to execute
    megaTelemetryFile->moveToThread(&fileWriterThread); // Needs a thread to execute
    QString megafilename = "megaTelemetry";
    megaTelemetryFile->createFile(megafilename);

    /** Set up the various pieces that handle and log the power switch controller **/
    QSerialPort* powerSerial = new QSerialPort(); // serial port for comms
    configureSerial(powerSerial,POWER_SERIAL_ARDU); // configured standard
    PowerSwitchArduino* myPower = new PowerSwitchArduino(powerSerial); // handles send/receive/signals
    telemetryClass* powerTelemetry = new telemetryClass(); // Power Telemetry Holder
    fileWriterClass* powerTelemetryFile = new fileWriterClass(); // Power Telemetry Logger
    myPower->moveToThread(&powerSerialThread); // Needs a thread to execute
    powerSerial->moveToThread(&powerSerialThread); // Needs a thread to execute
    powerTelemetry->moveToThread(&udpSenderThread); // Needs a thread to execute
    powerTelemetryFile->moveToThread(&fileWriterThread); // Needs a thread to execute
    QString powerfilename = "powerTelemetry";
    powerTelemetryFile->createFile(powerfilename);

    /** Connect Actions to Master Clock **/
//    QObject::connect()
    QObject::connect(masterClock,SIGNAL(doAction1Hz()),myMega,SLOT(requestTelemetry()));
//    QObject::connect(masterClock,SIGNAL(doAction5Hz()),mySender,SLOT(sendDatagram()));

    /** Connect Other Actions **/
    QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
    QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetryFile,SLOT(writeToFile(QByteArray)));
    QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
    QObject::connect(megaTelemetry,SIGNAL(reportTelemetryUpdate(QByteArray)),mySender,SLOT(sendDatagram(QByteArray)));

    QObject::connect(myPower,SIGNAL(signalPacketReceived(QByteArray)),powerTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
    QObject::connect(myPower,SIGNAL(signalPacketReceived(QByteArray)),powerTelemetryFile,SLOT(writeToFile(QByteArray)));
    QObject::connect(powerTelemetry,SIGNAL(reportTelemetryUpdate(QByteArray)),mySender,SLOT(sendDatagram(QByteArray)));

    /** Clean Up Classes on Thread Close **/
    QObject::connect(&masterClockThread,SIGNAL(finished()),masterClock,SLOT(deleteLater()));

    QObject::connect(&udpSenderThread,SIGNAL(finished()),mySender,SLOT(deleteLater()));
    QObject::connect(&udpListenerThread,SIGNAL(finished()),myListener,SLOT(deleteLater()));
    QObject::connect(&udpSenderThread,SIGNAL(finished()),megaTelemetry,SLOT(deleteLater()));
    QObject::connect(&udpSenderThread,SIGNAL(finished()),powerTelemetry,SLOT(deleteLater()));

    QObject::connect(&megaSerialThread,SIGNAL(finished()),myMega,SLOT(deleteLater()));
    QObject::connect(&megaSerialThread,SIGNAL(finished()),megaSerial,SLOT(deleteLater()));
    QObject::connect(&fileWriterThread,SIGNAL(finished()),megaTelemetryFile,SLOT(deleteLater()));

    QObject::connect(&powerSerialThread,SIGNAL(finished()),myPower,SLOT(deleteLater()));
    QObject::connect(&powerSerialThread,SIGNAL(finished()),powerSerial,SLOT(deleteLater()));
    QObject::connect(&fileWriterThread,SIGNAL(finished()),powerTelemetryFile,SLOT(deleteLater()));

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

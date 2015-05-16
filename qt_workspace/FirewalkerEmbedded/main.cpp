#include <QCoreApplication>
#include <QThread>
#include <QRegularExpressionMatch>
#include "masterclock.h"
#include "udppacketlistener.h"
#include "udppacketsender.h"
#include "megaserialclass.h"
#include "powerswitcharduino.h"
#include "telemetryclass.h"
#include "filewriterclass.h"

#define UNKNOWN_SERIAL_ONE "/dev/ttyUSB0"
#define UNKNOWN_SERIAL_TWO "/dev/ttyUSB1"
#define SERIAL_IDENTIFIER_MIN_LENGTH_BYTES 4

enum SerialDeviceType{
    SERIAL_TYPE_UNKNOWN
    , SERIAL_TYPE_MISSING
    , SERIAL_TYPE_MEGA
    , SERIAL_TYPE_POWER
};

int configureSerial(QSerialPort* serialPort, QString sPort)
{
    bool configured = false;
    int iTotalResponseLength = 0;
    int serialType = SERIAL_TYPE_UNKNOWN;

    QByteArray qbTemp;
    QByteArray qbResponse;
    serialPort->setPortName(sPort);
    serialPort->setBaudRate(QSerialPort::Baud57600);
    serialPort->open(QIODevice::ReadWrite);
    if( !serialPort->isOpen() )
        return SERIAL_TYPE_MISSING;
    /* Now ask the device what it is */
    serialPort->write("||UniqueId||"); // Frame characters are '|', and sending two frame chars ensures they work on reset or startup (bug workaround).
    while( !configured ) {
        qbTemp = serialPort->read(100);
        qbResponse.append(qbTemp);
        iTotalResponseLength += qbTemp.length();
        fprintf(stdout, "%.4d bytes: %s\n", qbTemp.length(), qbTemp.data());
        fflush(stdout);
        if( qbTemp.length() >= SERIAL_IDENTIFIER_MIN_LENGTH_BYTES )
            configured = true;
    }
    serialPort->flush();
    QRegularExpression reMega("(Mega)");
    if( reMega.match(qbResponse).hasMatch() )
        serialType = SERIAL_TYPE_MEGA;
    QRegularExpression rePower("(Power)");
    if( reMega.match(qbResponse).hasMatch() )
        serialType = SERIAL_TYPE_POWER;

    return serialType;
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

    megaSerialClass* myMega = NULL;             // initialize pointer
    telemetryClass* megaTelemetry = NULL;       // initialize pointer
    fileWriterClass* megaTelemetryFile = NULL;  // initialize pointer
    QString megafilename = "megaTelemetry";

    PowerSwitchArduino* myPower = NULL;             // initialize pointer
    telemetryClass* powerTelemetry = NULL;          // initialize pointer
    fileWriterClass* powerTelemetryFile = NULL;     // initialize pointer
    QString powerfilename = "powerTelemetry";

    MasterScheduler* masterClock = new MasterScheduler();
    masterClock->moveToThread(&masterClockThread); // Needs a thread to execute


    /** Set up the various pieces which handle remote communications **/
    udpPacketListener* myListener = new udpPacketListener();
    udpPacketSender* mySender = new udpPacketSender();
    mySender->moveToThread(&udpSenderThread);       // Needs a thread to execute
    myListener->moveToThread(&udpListenerThread);   // Needs a thread to execute

    /** Set up the serial ports, but we do not yet know which is which **/
    int unkSerType1, unkSerType2;
    QSerialPort* unknownSerial1 = new QSerialPort();                    // serial port for comms
    unkSerType1 = configureSerial(unknownSerial1,UNKNOWN_SERIAL_ONE);   // configured standard
    QSerialPort* unknownSerial2 = new QSerialPort();                    // serial port for comms
    unkSerType2 = configureSerial(unknownSerial2,UNKNOWN_SERIAL_TWO);   // configured standard
    /* Assign the serial ports */
    QSerialPort* megaSerial = NULL;         // these are just pointers
    QSerialPort* powerSerial = NULL;        // these are just pointers -- there's going to be two pointers to each good serial port
    if( unkSerType1 == SERIAL_TYPE_MEGA )
        megaSerial = unknownSerial1;
    if( unkSerType2 == SERIAL_TYPE_MEGA )
        megaSerial = unknownSerial2;
    if( unkSerType1 == SERIAL_TYPE_POWER )
        powerSerial = unknownSerial1;
    if( unkSerType2 == SERIAL_TYPE_POWER )
        powerSerial = unknownSerial2;

    bool bMegaSerialUnassignedFlag, bPowerSerialUnassignedFlag;

    /** Set up the various pieces that handle and log the sensor readings and telemetry **/
    if( megaSerial != NULL )
    {
        bMegaSerialUnassignedFlag = false;
        myMega = new megaSerialClass(powerSerial);      // handles send/receive/signals
        megaTelemetry = new telemetryClass();           // Sensor Telemetry Holder
        megaTelemetryFile = new fileWriterClass();      // Sensor Telemetry Logger
        myMega->moveToThread(&megaSerialThread);        // Needs a thread to execute
        megaSerial->moveToThread(&megaSerialThread);    // Needs a thread to execute
        megaTelemetry->moveToThread(&udpSenderThread);  // Needs a thread to execute
        megaTelemetryFile->moveToThread(&fileWriterThread);     // Needs a thread to execute
        megaTelemetryFile->createFile(megafilename);
    } else {
        bMegaSerialUnassignedFlag = true;
    }

    /** Set up the various pieces that handle and log the power switch controller **/
    if( powerSerial != NULL )
    {
        bPowerSerialUnassignedFlag = false;
        myPower = new PowerSwitchArduino(powerSerial);      // handles send/receive/signals
        powerTelemetry = new telemetryClass();              // Power Telemetry Holder
        powerTelemetryFile = new fileWriterClass();         // Power Telemetry Logger
        myPower->moveToThread(&powerSerialThread);          // Needs a thread to execute
        powerSerial->moveToThread(&powerSerialThread);      // Needs a thread to execute
        powerTelemetry->moveToThread(&udpSenderThread);     // Needs a thread to execute
        powerTelemetryFile->moveToThread(&fileWriterThread);    // Needs a thread to execute
        powerTelemetryFile->createFile(powerfilename);
    } else {
        bPowerSerialUnassignedFlag = true;
    }

    /** Connect Actions to Master Clock **/
//    QObject::connect()
    if( !bMegaSerialUnassignedFlag )
        QObject::connect(masterClock,SIGNAL(doAction1Hz()),myMega,SLOT(requestTelemetry()));

//    QObject::connect(masterClock,SIGNAL(doAction5Hz()),mySender,SLOT(sendDatagram()));

    /** Connect Other Actions **/
    if( !bMegaSerialUnassignedFlag )
    {
        QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
        QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetryFile,SLOT(writeToFile(QByteArray)));
        QObject::connect(myMega,SIGNAL(signalPacketReceived(QByteArray)),megaTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
        QObject::connect(megaTelemetry,SIGNAL(reportTelemetryUpdate(QByteArray)),mySender,SLOT(sendDatagram(QByteArray)));
    }

    if( !bPowerSerialUnassignedFlag )
    {
        QObject::connect(myPower,SIGNAL(signalPacketReceived(QByteArray)),powerTelemetry,SLOT(parseTelemetryPacket(QByteArray)));
        QObject::connect(myPower,SIGNAL(signalPacketReceived(QByteArray)),powerTelemetryFile,SLOT(writeToFile(QByteArray)));
        QObject::connect(powerTelemetry,SIGNAL(reportTelemetryUpdate(QByteArray)),mySender,SLOT(sendDatagram(QByteArray)));
    }

    /** Clean Up Classes on Thread Close **/
    QObject::connect(&masterClockThread,SIGNAL(started()),masterClock,SLOT(doWork()));
    QObject::connect(&masterClockThread,SIGNAL(finished()),masterClock,SLOT(deleteLater()));

    QObject::connect(&udpSenderThread,SIGNAL(finished()),mySender,SLOT(deleteLater()));
    QObject::connect(&udpListenerThread,SIGNAL(finished()),myListener,SLOT(deleteLater()));
    QObject::connect(&udpSenderThread,SIGNAL(finished()),megaTelemetry,SLOT(deleteLater()));
    QObject::connect(&udpSenderThread,SIGNAL(finished()),powerTelemetry,SLOT(deleteLater()));

    if( !bMegaSerialUnassignedFlag )
    {
        QObject::connect(&megaSerialThread,SIGNAL(finished()),myMega,SLOT(deleteLater()));
        QObject::connect(&megaSerialThread,SIGNAL(finished()),megaSerial,SLOT(deleteLater()));
        QObject::connect(&fileWriterThread,SIGNAL(finished()),megaTelemetryFile,SLOT(deleteLater()));
    }

    if( !bPowerSerialUnassignedFlag )
    {
        QObject::connect(&powerSerialThread,SIGNAL(finished()),myPower,SLOT(deleteLater()));
        QObject::connect(&powerSerialThread,SIGNAL(finished()),powerSerial,SLOT(deleteLater()));
        QObject::connect(&fileWriterThread,SIGNAL(finished()),powerTelemetryFile,SLOT(deleteLater()));
    }

//        masterClock->doWork();

    masterClockThread.start();
    udpListenerThread.start();
    udpSenderThread.start();
    if( !bMegaSerialUnassignedFlag )
        megaSerialThread.start();
    fileWriterThread.start();

    /* Set thread priority after starting thread */
    masterClockThread.setPriority(QThread::HighestPriority);

    return a.exec();
}

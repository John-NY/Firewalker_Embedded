/******************************************************************************
 * Arduino Serial Class
 *
 * This class handles all read/write operations with the sensor module.
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#include "powerswitcharduino.h"

PowerSwitchArduino::PowerSwitchArduino(QSerialPort *serialPort, QObject *parent) : QObject(parent)
  , m_serialPort(serialPort)
  , m_standardOutput(stdout)
  , m_bytesWritten(0)
{
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    connect(m_serialPort, SIGNAL(bytesWritten(qint64)), this, SLOT(handleBytesWritten(qint64)));
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
//    connect(&m_timer, SIGNAL(timeout()), SLOT(handleReaderTimeout()));
    connect(this,SIGNAL(signalReaderTimer()),this,SLOT(startReaderTimer()));

//    m_timer.moveToThread((this->thread())); // necessary
}

PowerSwitchArduino::~PowerSwitchArduino()
{

}

void PowerSwitchArduino::handleReadyRead()
{
//    m_standardOutput << ",";
    m_readData.append(m_serialPort->readAll());

//    if (!m_timer.isActive())
    emit signalReaderTimer();
//        m_timer.start(100);
}

void PowerSwitchArduino::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size()) {
        m_bytesWritten = 0;
        m_standardOutput << QObject::tr("\nData successfully sent to port %1").arg(m_serialPort->portName()) << endl;
        m_standardOutput << endl;
    }
}

void PowerSwitchArduino::handleReaderTimeout()
{
//    m_standardOutput << ",";
    if (m_readData.isEmpty()) {
        m_standardOutput << QObject::tr("No data was currently available for reading from port %1").arg(m_serialPort->portName()) << endl;
    } else {
//        m_standardOutput << QObject::tr("Data successfully received from port %1").arg(m_serialPort->portName()) << endl;
//        m_standardOutput << m_readData << endl;
        m_accumulated.append(m_readData);
        m_readData.clear();
        ProcessSerialReceived();
    }
}

void PowerSwitchArduino::ProcessSerialReceived()
{
    int start = -1;
    int end = -1;
    bool hasStarted = false;
    for (int i = 0; i < m_accumulated.length(); i++)
    {
        // Check whether there's an end char -- do this first
        if( (m_accumulated.at(i) == FEND) && hasStarted )
        {
            end = i;
            break;
        }
        // Next, check for "start" char -- do this last
        if( (m_accumulated.at(i) == FEND) && (!hasStarted) )
        {
            start = i;
            hasStarted = true;
        }
    }
//    m_standardOutput << "?";
    if( (start > -1) && (end > -1) )
    {
//        m_standardOutput << "?";
        emit signalPacketReceived( m_accumulated.mid(start,end-start+1) );
        m_accumulated.remove(start,end-start+1);
//        m_accumulated.clear();
    }
}

void PowerSwitchArduino::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
    if (serialPortError == QSerialPort::WriteError) {
        m_standardOutput << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
}

void PowerSwitchArduino::write(const QByteArray &writeData)
{
    m_writeData = writeData;

    qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    }
}

void PowerSwitchArduino::requestTelemetry()
{
    write("|T|");
}

void PowerSwitchArduino::startReaderTimer()
{
//    m_timer.start(100);
    handleReaderTimeout();
}

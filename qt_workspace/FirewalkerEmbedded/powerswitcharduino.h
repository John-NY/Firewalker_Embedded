/******************************************************************************
 * Arduino Serial Class
 *
 * This class handles all read/write operations with the sensor module.
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#ifndef POWERSWITCHARDUINO_H
#define POWERSWITCHARDUINO_H

#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>

#define FEND '|'

class PowerSwitchArduino : public QObject
{
    Q_OBJECT
public:
    explicit PowerSwitchArduino(QSerialPort* serialPort, QObject *parent = 0);
    ~PowerSwitchArduino();

private:
    QSerialPort     *m_serialPort;
    QByteArray  m_readData;
    QByteArray      m_writeData;
    QTextStream m_standardOutput;
    qint64          m_bytesWritten;
    QByteArray m_accumulated;

    void write(const QByteArray &writeData);
    void configureSerial(QSerialPort* serialPort);
    void ProcessSerialReceived();

signals:
    void signalReaderTimer();
    void signalPacketReceived(QByteArray);

public slots:
    void requestTelemetry();

private slots:
    void startReaderTimer();
    void handleReadyRead();
    void handleBytesWritten(qint64 bytes);
    void handleReaderTimeout();
    void handleError(QSerialPort::SerialPortError error);
};

#endif // POWERSWITCHARDUINO_H

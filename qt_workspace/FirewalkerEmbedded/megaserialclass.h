#ifndef MEGASERIALCLASS_H
#define MEGASERIALCLASS_H

#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>

#define FEND '|'

class megaSerialClass : public QObject
{
    Q_OBJECT
public:
    explicit megaSerialClass(QSerialPort* serialPort, QObject *parent = 0);
    ~megaSerialClass();

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

#endif // MEGASERIALCLASS_H

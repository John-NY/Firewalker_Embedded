#ifndef MASTERCLOCK_H
#define MASTERCLOCK_H

#include <QObject>
#include <QTimer>
#include <time.h>
#include <stdio.h>

class MasterScheduler : public QObject
{
    Q_OBJECT
public:
    explicit MasterScheduler(QObject *parent = 0);
    ~MasterScheduler();

private:
    bool _finished;
    int _counter;
    float _frequency_Hz;
    float _Period_ms;
    int _Period_1Hz_tics;
    int _Period_5Hz_tics;
    struct timespec ts;

signals:
    void doAction1Hz();
    void doAction5Hz();

public slots:
    void doWork();

};

#endif // MASTERCLOCK_H

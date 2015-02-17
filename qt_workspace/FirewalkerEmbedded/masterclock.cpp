#include "masterclock.h"

MasterScheduler::MasterScheduler(QObject *parent) : QObject(parent)
{
    _counter = 0;
    _frequency_Hz = 100.0; // 10 ms timer
    _Period_ms = 1000.0/_frequency_Hz;
    _Period_1Hz_tics = int(_frequency_Hz/1);
    _Period_5Hz_tics = int(_frequency_Hz/5);
    ts = {0,int(_Period_ms*1000*1000)};
    _finished = false;

}

MasterScheduler::~MasterScheduler()
{

}

void MasterScheduler::doWork()
{
    fprintf(stdout,"starting master clock\n");
    while( !_finished )
    {
        _counter++;
//        fprintf(stdout,"%d,",_counter);
        if( (_counter % _Period_1Hz_tics) == 0 )
        {
            emit doAction1Hz();
        }
        if( (_counter % _Period_5Hz_tics) == 0 )
        {
            emit doAction5Hz();
        }
        // http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
        nanosleep(&ts,NULL);
    }

}


/******************************************************************************
 * File Writer Class
 *
 * This class writes to a single file.  There may be multiple instances
 * of this class for writing to different files/logs.
 * It probably makes sense to assign all instances of this class to a
 * single thread.
 *
 * The next incarnation of this is to create a new file every minute [TBR-01].
 *
 * TBR-01 : Create a new file every 1 (5? 10?) minute(s).
 *
 * Created 16 Feb 2015 by John Donovan for Ted Donovan (SDSM)
 *****************************************************************************/
#include "filewriterclass.h"

fileWriterClass::fileWriterClass(QObject *parent) : QObject(parent)
{

}

fileWriterClass::~fileWriterClass()
{

}

QFile *fileWriterClass::createFile(QString &name)
{
    fileName.sprintf("%s_%s.txt",t.currentDateTime().toString("yyyyMMdd-HHmmss").toLocal8Bit().data(), name.toLocal8Bit().data());
    fprintf(stdout,"%s\n",fileName.toLocal8Bit().data());
    myFile = new QFile(fileName);
    myFile->open(QFile::WriteOnly);
    if( !myFile->isOpen() )
        fprintf(stdout,"Could not open file %s!", fileName.data());
}

void fileWriterClass::writeToFile(QByteArray myData)
{
    QString myTelemetry(myData);
    myTelemetry.replace("|","");
    myTelemetry.prepend(t.currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzzz, "));
    myTelemetry.append("\n");
    myFile->write(myTelemetry.toLocal8Bit());
    myFile->flush();
}

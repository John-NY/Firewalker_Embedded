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
#ifndef FILEWRITERCLASS_H
#define FILEWRITERCLASS_H

#include <QObject>
#include <QFile>
#include <QDateTime>

class fileWriterClass : public QObject
{
    Q_OBJECT
public:
    explicit fileWriterClass(QObject *parent = 0);
    ~fileWriterClass();
    QFile *createFile(QString& name);

private:
    QDateTime t;
    QFile* myFile;
    QString fileName;
//    void setFolder();

signals:

public slots:
    void writeToFile(QByteArray);

};

#endif // FILEWRITERCLASS_H

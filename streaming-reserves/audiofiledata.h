/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef AUDIOFILEDATA_H
#define AUDIOFILEDATA_H
#include <QObject>
#include <QProcess>

/* This class holds metadata for a single audio file
 */

class audioFileData //: public QObject
{
public:
    audioFileData();
    bool readFileInfo(QString dataFile);
    QString title;
    QString artist;
    QString album;
    QString composer;
    QString track;
    QString filename;
    QString originalFilename;

    QString format;
    QString year;
    QString displayTitle;
    QWidget * parent;
    QString cdNumber;

private:
    QProcess * mp3tagProcess;
    QString infoFile;
    QString processPath;
};

#endif // AUDIOFILEDATA_H

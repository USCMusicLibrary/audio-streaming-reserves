/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "audiofiledata.h"
#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <fstream>
#include <string>
#include <sstream>
#include "utils.h"

audioFileData::audioFileData()
{
    //parent = widget;
    title = "";
    artist = "";
    album = "";
    track = "";
    composer = "";
    infoFile = "";
    filename = "";
    originalFilename = "";
    format = "";
    year = "";
    //processPath = QApplication::applicationDirPath() + "/tagcmd.b";
}

bool audioFileData::readFileInfo(QString dataFile){
/*
    QStringList arguments;

    mp3tagProcess = new QProcess(this);

    mp3tagProcess->start(processPath,arguments);
    mp3tagProcess->waitForFinished();
    QMessageBox::about(parent,"debug","finished batch file");
    */
    std::ifstream file;
    file.open(dataFile.toStdString());
    std::string tmpString;
    for (int i=0; i<5; i++){//read header info and dump
        std::getline(file,tmpString);
        //QMessageBox::about(parent,"debug",tmpString.c_str());
    }

    //get format
    std::getline(file,tmpString);
    std::stringstream stream (tmpString);
    char delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    format = tmpString.c_str();

    //dump extra info
    std::getline(file,tmpString);
    std::getline(file,tmpString);


    //get title
    std::getline(file,tmpString);
    stream.clear();
    stream.str(tmpString);
    delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    title = tmpString.c_str();
    displayTitle = title;

    //get artist
    std::getline(file,tmpString);
    stream.clear();
    stream.str(tmpString);
    delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    artist = tmpString.c_str();

    //get album
    std::getline(file,tmpString);
    stream.clear();
    stream.str(tmpString);
    delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    album = tmpString.c_str();

    //get year
    std::getline(file,tmpString);
    stream.clear();
    stream.str(tmpString);
    delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    year = tmpString.c_str();

    //get track no.
    std::getline(file,tmpString);
    stream.clear();
    stream.str(tmpString);
    delim=':';
    std::getline(stream,tmpString,delim);
    std::getline(stream,tmpString);
    tmpString = Trim(tmpString);
    //QMessageBox::about(parent,"debug",tmpString.c_str());
    track = tmpString.c_str();

    file.close();

    return true;
}

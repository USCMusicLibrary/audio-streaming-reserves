/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QJsonDocument>
#include <vector>
#include <QFile>

#include "audiofiledata.h"
#include "playlist.h"
#include "category.h"

/* main library class
 * contains two main vectors:
 *   one for all the tracks in the library (metadata)
 *   another for all the categories (which include playlists)
 */

class library
{

public:
    explicit library(QString jsonFileName="",QWidget *parent=NULL);

    std::vector<audioFileData> tracks;
    std::vector<category> categories;
    void sortCategories();

    bool saveJsonFile(QString &error,QString fileName="");

    //library (QString jsonFileName);//overloaded constructor for json file



    bool updateLibrary();

    QJsonDocument readJsonFile(QString fileName, QString &error);
    bool readTracksFromJson(QJsonArray tracksArray, QString &error);
    bool readCategoriesFromJson(QJsonArray categoriesArray, QString &error);
    QJsonDocument jsonDoc;

    QString jsonSaveFileName;

    QString error;

};

#endif // LIBRARY_H

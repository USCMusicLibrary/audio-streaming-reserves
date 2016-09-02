/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "library.h"
#include <QApplication>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>

/* constructor
 * loads json library data from library file
 */
library::library(QString jsonFileName, QWidget *parent)
{

    if (jsonFileName=="" || parent==NULL) return; //return if not proper parameters
    //TODO: if not proper parameters then set an error in error string

    jsonDoc = readJsonFile(jsonFileName,error);
    jsonSaveFileName = jsonFileName;


    //make sure json doc is valid
    if (jsonDoc.isNull()){
        //QMessageBox::about(parent,"debug","is null");
        throw QString("Json file is invalid:") + error;
    }
    if (jsonDoc.isObject()){
        QJsonObject documentObject = jsonDoc.object();
        if (!documentObject.contains("tracks")) throw QString("Json document does not contain 'tracks'.");
        QJsonValue tracksObject = documentObject.value("tracks");
        if (!tracksObject.isArray() || tracksObject.isUndefined()) throw QString("Json document contains error in 'tracks'.");

        if (!readTracksFromJson(tracksObject.toArray(),error)) throw QString("Unable to read track information. "+ error);

        if (!documentObject.contains("categories")) throw QString("Json document does not contain 'categories'.");
        QJsonValue categoriesObject = documentObject.value("categories");
        if (!categoriesObject.isArray() || categoriesObject.isUndefined()) throw QString("Json document contains error in 'categories'.");


        if (!readCategoriesFromJson(categoriesObject.toArray(),error)) throw QString("Unable to read categories information. "+ error);

    }



}

void library::sortCategories(){
    std::sort(this->categories.begin(),this->categories.end());
}

bool library::updateLibrary(){

    QString jsonFileName = jsonSaveFileName;
    jsonDoc = readJsonFile(jsonFileName,error);




    //make sure json doc is valid
    if (jsonDoc.isNull()){
        //QMessageBox::about(parent,"debug","is null");
        throw QString("Json file is invalid:") + error;
    }

    this->categories.clear();
    this->tracks.clear();

    if (jsonDoc.isObject()){
        QJsonObject documentObject = jsonDoc.object();
        if (!documentObject.contains("tracks")) throw QString("Json document does not contain 'tracks'.");
        QJsonValue tracksObject = documentObject.value("tracks");
        if (!tracksObject.isArray() || tracksObject.isUndefined()) throw QString("Json document contains error in 'tracks'.");

        if (!readTracksFromJson(tracksObject.toArray(),error)) throw QString("Unable to read track information. "+ error);

        if (!documentObject.contains("categories")) throw QString("Json document does not contain 'categories'.");
        QJsonValue categoriesObject = documentObject.value("categories");
        if (!categoriesObject.isArray() || categoriesObject.isUndefined()) throw QString("Json document contains error in 'categories'.");


        if (!readCategoriesFromJson(categoriesObject.toArray(),error)) throw QString("Unable to read categories information. "+ error);

    }


}

QJsonDocument library::readJsonFile(QString fileName, QString &error){
    error = "";
    fileName = QApplication::applicationDirPath()  + "/" + fileName;
    QFile file(fileName);
    QByteArray data;
    QJsonDocument newJsonDoc;
    if (!file.open(QIODevice::ReadOnly)){
        error = "Unable to open library file.";
        return newJsonDoc;
    }
    data = file.readAll();
    //QString dataString = QString(data);
    /*while (!file.atEnd()) {
        QByteArray line = file.readLine();
        data.append(line);
    }*/
    QJsonParseError err;
    //wJsonDoc = QJsonDocument::fromBinaryData(data);
    newJsonDoc = QJsonDocument::fromJson(data,&err);
    error = err.errorString();
    file.close();
    return newJsonDoc;
}

bool library::saveJsonFile(QString &error, QString fileName){
    error = "";
    QJsonDocument newDoc;
    QJsonObject mainDoc;
    QJsonArray tracksArray;

    for (audioFileData track : tracks){
        QJsonObject trackObject;
        trackObject.insert("title",track.title);
        trackObject.insert("composer",track.composer);
        trackObject.insert("artist",track.artist);
        trackObject.insert("album",track.album);
        trackObject.insert("track",track.track);
        trackObject.insert("filename",track.filename);
        trackObject.insert("originalFilename",track.originalFilename);
        trackObject.insert("format",track.format);
        trackObject.insert("year",track.year);
        trackObject.insert("displayTitle",track.displayTitle);
        trackObject.insert("cdNumber",track.cdNumber);
        tracksArray.push_back(trackObject);
    }
    mainDoc.insert("tracks",tracksArray);


    QJsonArray categoriesArray;

    for (category currentCategory : categories) {
        QJsonObject categoryObject;
        categoryObject.insert("name",currentCategory.categoryID);
        categoryObject.insert("title",currentCategory.categoryDisplayName);
        categoryObject.insert("description",currentCategory.description);
        QJsonArray playlistsArray;
        for (playlist currentPlaylist : currentCategory.playlists){
            QJsonObject playlistObject;
            playlistObject.insert("type",currentPlaylist.type);
            playlistObject.insert("name",currentPlaylist.playlistID);
            playlistObject.insert("title",currentPlaylist.playlistDisplayName);
            playlistObject.insert("description",currentPlaylist.description);
            playlistObject.insert("permalink",currentPlaylist.permalink);
            playlistObject.insert("displayDividerTitle",currentPlaylist.displayDividerTitle);
            playlistObject.insert("displayDividerHR",currentPlaylist.displayDividerHR);
            QJsonArray trackArray;
            for (int i=0; i<currentPlaylist.playlistTracks.size(); i++){
                QString trackID = currentPlaylist.playlistTracks.at(i);
                trackArray.push_back(trackID);
            }
            playlistObject.insert("tracks",trackArray);
            playlistsArray.push_back(playlistObject);
        }
        categoryObject.insert("playlists",playlistsArray);
        categoriesArray.push_back(categoryObject);
    }

    mainDoc.insert("categories",categoriesArray);



    newDoc.setObject(mainDoc);
    QByteArray data = newDoc.toJson();

    if (fileName=="") fileName=jsonSaveFileName;

    fileName = QApplication::applicationDirPath()  + "/" + fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)){
        error = "Unable to open library save file.";
        return false;
    }
    file.write(data);
    file.close();


    return true;
}


bool library::readCategoriesFromJson(QJsonArray categoriesArray, QString &error){
    error = "";
    categories.clear();
    for (int i=0; i<categoriesArray.size(); i++){
        QJsonValue categoryValue = categoriesArray.at(i);
        if (!categoryValue.isObject() || categoryValue.isUndefined()){error = "Error reading category array. Check Json file."; return false;}
        QJsonObject categoryObject = categoryValue.toObject();
        //QString categoryName;

        category currentCategory;

        QJsonValue tmpValue = categoryObject.value("name");
        if (!tmpValue.isString()){error = "Error reading category name. Check Json file."; return false;}
        currentCategory.categoryID = tmpValue.toString();

        tmpValue = categoryObject.value("title");
        if (!tmpValue.isString()){error = "Error reading category title. Check Json file."; return false;}
        currentCategory.categoryDisplayName = tmpValue.toString();

        tmpValue = categoryObject.value("description");
        if (!tmpValue.isString()){error = "Error reading category description. Check Json file."; return false;}
        currentCategory.description = tmpValue.toString();

        std::vector<playlist> categoryPlaylists; //Do we need this object??
        if (!categoryObject.contains("playlists")) {error = "Json document does not contain 'playlists'."; return false;}
        QJsonValue playlistsObject = categoryObject.value("playlists");
        if (!playlistsObject.isArray() || playlistsObject.isUndefined()) { error = "Json document contains error in 'categories'."; return false;}
        QJsonArray playlistsArray = playlistsObject.toArray();
        for (int j=0; j<playlistsArray.size(); j++){
            QJsonValue playlistValue = playlistsArray.at(j);
            if (!playlistValue.isObject() || playlistValue.isUndefined()){error = "Error reading category array. Check Json file."; return false;}
            QJsonObject playlistObject = playlistValue.toObject();
            playlist currentPlaylist;

            tmpValue = playlistObject.value("type");
            if (!tmpValue.isString()){error = "Error reading playlist type. Check Json file."; return false;}
            currentPlaylist.type = tmpValue.toString();

            tmpValue = playlistObject.value("name");
            if (!tmpValue.isString()){error = "Error reading playlist name. Check Json file."; return false;}
            currentPlaylist.playlistID = tmpValue.toString();

            tmpValue = playlistObject.value("title");
            if (!tmpValue.isString()){error = "Error reading playlist title. Check Json file."; return false;}
            currentPlaylist.playlistDisplayName = tmpValue.toString();

            tmpValue = playlistObject.value("description");
            if (!tmpValue.isString()){error = "Error reading playlist description. Check Json file."; return false;}
            currentPlaylist.description = tmpValue.toString();

            tmpValue = playlistObject.value("permalink");
            if (!tmpValue.isString()){error = "Error reading playlist permalink. Check Json file."; return false;}
            currentPlaylist.permalink = tmpValue.toString();

            tmpValue = playlistObject.value("displayDividerHR");
            if (!tmpValue.isBool()){error = "Error reading playlist divider. Check Json file."; return false;}
            currentPlaylist.displayDividerHR = tmpValue.toBool();

            tmpValue = playlistObject.value("displayDividerTitle");
            if (!tmpValue.isBool()){error = "Error reading playlist divider. Check Json file."; return false;}
            currentPlaylist.displayDividerTitle = tmpValue.toBool();

            QStringList tracks;
            if (!playlistObject.contains("tracks")) {error = "Json document does not contain 'tracks' in playlist."; return false;}
            QJsonValue tracksObject = playlistObject.value("tracks");
            if (!tracksObject.isArray() || tracksObject.isUndefined()) { error = "Json document contains error in 'categories'."; return false;}
            QJsonArray tracksArray = tracksObject.toArray();

            for (int k=0; k<tracksArray.size(); k++){
                QJsonValue trackValue = tracksArray.at(k);
                if (!trackValue.isString() || trackValue.isUndefined()){error = "Error reading track array in playlist. Check Json file."; return false;}
                QString trackID = trackValue.toString();
                tracks.push_back(trackID);
            }

            currentPlaylist.playlistTracks = tracks;
            currentCategory.playlists.push_back(currentPlaylist);
        }
        categories.push_back(currentCategory);

    }
    return true;
}

bool library::readTracksFromJson(QJsonArray tracksArray, QString &error){
    error="";
    tracks.clear();
    for (int i=0; i<tracksArray.size(); i++){
        QJsonValue track = tracksArray.at(i);
        if (!track.isObject() || track.isUndefined()){error = "Error reading tracks array. Check Json file."; return false;}
        QJsonObject trackObject = track.toObject();
        audioFileData currentTrack;

        QJsonValue tmpValue = trackObject.value("title");
        if (!tmpValue.isString()){error = "Error reading track title. Check Json file."; return false;}
        currentTrack.title = tmpValue.toString();

        tmpValue = trackObject.value("composer");
        if (!tmpValue.isString()){error = "Error reading track composer. Check Json file."; return false;}
        currentTrack.composer = tmpValue.toString();

        tmpValue = trackObject.value("artist");
        if (!tmpValue.isString()){error = "Error reading track artist. Check Json file."; return false;}
        currentTrack.artist = tmpValue.toString();

        tmpValue = trackObject.value("album");
        if (!tmpValue.isString()){error = "Error reading track album. Check Json file."; return false;}
        currentTrack.album = tmpValue.toString();

        tmpValue = trackObject.value("track");
        if (!tmpValue.isString()){error = "Error reading track number. Check Json file."; return false;}
        currentTrack.track = tmpValue.toString();

        tmpValue = trackObject.value("filename");
        if (!tmpValue.isString()){error = "Error reading track filename. Check Json file."; return false;}
        currentTrack.filename = tmpValue.toString();

        tmpValue = trackObject.value("originalFilename");
        if (!tmpValue.isString()){error = "Error reading track original filename. Check Json file."; return false;}
        currentTrack.originalFilename = tmpValue.toString();

        tmpValue = trackObject.value("format");
        if (!tmpValue.isString()){error = "Error reading track format. Check Json file."; return false;}
        currentTrack.format = tmpValue.toString();

        tmpValue = trackObject.value("year");
        if (!tmpValue.isString()){error = "Error reading track year. Check Json file."; return false;}
        currentTrack.year = tmpValue.toString();

        tmpValue = trackObject.value("displayTitle");
        if (!tmpValue.isString()){error = "Error reading track display title. Check Json file."; return false;}
        currentTrack.displayTitle = tmpValue.toString();

        tmpValue = trackObject.value("cdNumber");
        if (!tmpValue.isString()){error = "Error reading track CD number. Check Json file: "+currentTrack.title; return false;}
        currentTrack.cdNumber = tmpValue.toString();

        tracks.push_back(currentTrack);
    }


    return true;
}

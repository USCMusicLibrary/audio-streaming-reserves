/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "playlist.h"

playlist::playlist()
{
    QDateTime dt=QDateTime::currentDateTimeUtc();
    QString dtStr = dt.toString("yyyyMMddhhmmss");
    this->playlistID = dtStr;
    playlistDisplayName = "";
    type = "playlist";
    displayDividerTitle = false;
    displayDividerHR = false;
}

bool playlist::addNewTrack(QString trackID){
    playlistTracks.push_back(trackID);
    return true;
}

bool playlist::remove(int index){
    playlistTracks.removeAt(index);
    return true;
}

bool playlist::moveUp(int index){
    QString tmpID=playlistTracks.at(index);
    playlistTracks.removeAt(index);
    playlistTracks.insert(index-1,tmpID);
    return true;
}

bool playlist::moveDown(int index){
    QString tmpID=playlistTracks.at(index);
    playlistTracks.removeAt(index);
    playlistTracks.insert(index+1,tmpID);
    return true;
}

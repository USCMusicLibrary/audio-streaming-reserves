/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "category.h"

category::category()
{
    QDateTime dt=QDateTime::currentDateTimeUtc();
    QString dtStr = dt.toString("yyyyMMddhhmmss");
    this->categoryID = dtStr;
    categoryDisplayName = "new category";
    description = "";
}

bool category::moveUp(int index){
    if (index==0)
        return false;
    playlist tmpID=playlists.at(index);
    playlists.erase(playlists.begin()+index);
    //playlists.removeAt(index);
    playlists.insert(playlists.begin()+index-1,tmpID);
    //playlists.insert(index-1,tmpID);
    return true;
}

bool category::moveDown(int index){
    if (index==playlists.size()-1)
        return false;
    playlist tmpID=playlists.at(index);
    //playlistTracks.erase(index);
    playlists.erase(playlists.begin()+index);
    //playlistTracks.insert(index+1,tmpID);
    playlists.insert(playlists.begin()+index+1,tmpID);
    return true;
}

bool category::operator <(const category& right){
    return this->categoryDisplayName<right.categoryDisplayName;
}

/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QProcess>
#include <vector>
#include <QDateTime>

/* playlist class
 * contains metadata for a single playlist
 *   in some cases also a QStringList of track ids
 */
class playlist
{
public:
    playlist();

    QString playlistDisplayName;
    QString playlistID; //TODO: call function in library to generate new playlist ID
    QStringList playlistTracks;//filenames/id of tracks in playlist


    QString type; //playlist, permalink, divider
    QString description;
    QString permalink;
    bool displayDividerTitle;
    bool displayDividerHR;


    bool addNewTrack(QString trackID);
    bool remove(int index);
    bool moveUp(int index);
    bool moveDown(int index);

};

#endif // PLAYLIST_H

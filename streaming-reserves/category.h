/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef CATEGORY_H
#define CATEGORY_H

#include <vector>
#include <QObject>
#include "playlist.h"
#include <QDateTime>

/* this class contains metadata for a single category and also a
 * vectory of playlists in that category
 */

class category
{
public:
    category();

    QString categoryDisplayName; //title
    QString categoryID; //name - auto generated
    //QStringList playlists; //array of playlist ids/names

    QString description;
    std::vector <playlist> playlists;

    bool moveUp(int index);
    bool moveDown(int index);

    bool operator <(const category &right);

};

#endif // CATEGORY_H

/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "tracksTableModel.h"
#include <QMessageBox>

/*trackTableModel::trackTableModel(QObject *parent) :
    QObject(parent)
{
}
*/


int tracksTableModel::rowCount(const QModelIndex &parent) const
{
    return this->Library->tracks.size();
}
int tracksTableModel::columnCount(const QModelIndex &parent) const
{
    return 9;//number of columns doesn't change
}

QVariant tracksTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        QString header;
    switch (section){
        case 0:
            header = "CD Number";
            break;
        case 1:
            header = "Artist";
            break;
        case 2:
            header = "Title";
            break;
        case 3:
            header = "Album";
            break;
        case 4:
            header = "Display Title";
            break;
        case 5:
            header = "Filename";
            break;
        case 6:
            header = "Track";
            break;
        case 7:
            header = "Year";
            break;
        case 8:
            header = "Original Filename";
            break;
       }
    return header;
    }
    else return QVariant();
}

QVariant tracksTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){
        audioFileData *track = &Library->tracks.at(index.row());
        QVariant data;
        switch (index.column()){
            case 0:
                data = track->cdNumber.toInt();
                break;
            case 1:
                data = track->artist;
                break;
            case 2:
                data = track->title;
                break;
            case 3:
                data = track->album;
                break;
            case 4:
                data = track->displayTitle;
                break;
            case 5:
                data = track->filename;
                break;
            case 6:
                data = track->track;
                break;
            case 7:
                data = track->year;
                break;
            case 8:
                data = track->originalFilename;
                break;

        }
        return data;
    }
    return QVariant();
}


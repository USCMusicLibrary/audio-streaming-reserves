/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef TRACKSTABLEMODEL_H
#define TRACKSTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDragEnterEvent>
#include <QMimeData>

#include "library.h"

/* tracksTableModelClass
 * this class is used to override the default model in tableView to add custom functions
 */

class tracksTableModel : public QAbstractTableModel
{
public:
    /*int rowCount(const QModelIndex &parent = QModelIndex()) const
        {
            return 2;
        }
        int columnCount(const QModelIndex &parent = QModelIndex()) const
        {
            return 2;
        }
        QVariant data(const QModelIndex &index, int role) const
        {
            switch (role)
            {
            case Qt::DisplayRole:
            {
                return 4 - index.row() + index.column();
            }
            }
            return QVariant();
        }*/
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QVariant data(const QModelIndex &index, int role) const;


private:
   library *Library;

public:
   int setLibraryPointer(library * lib = NULL){
       this->Library = lib;
       return 0;
   }
};

#endif // TRACKSTABLEMODEL_H

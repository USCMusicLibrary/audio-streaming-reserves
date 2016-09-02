/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include "library.h"

class updater : public QObject
{
    Q_OBJECT
public:
    explicit updater(QObject *parent = 0);

signals:
    void dataUpdated(const QString &result);

public slots:
    void checkForUpdates(library & library);

};

#endif // UPDATER_H

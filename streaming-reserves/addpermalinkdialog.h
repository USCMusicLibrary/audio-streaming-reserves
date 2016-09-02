/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef ADDPERMALINKDIALOG_H
#define ADDPERMALINKDIALOG_H

#include <QDialog>
#include "mainwindow.h"
#include "category.h"
#include "playlist.h"

namespace Ui {
class addPermalinkDialog;
}

class addPermalinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addPermalinkDialog(QWidget *parent = 0, library &Library =library(), bool edit = false, int catEditIndex=-1,int playlistEditIndex=-1);
    ~addPermalinkDialog();

    library &libraryNew;
    bool editDialog;
    int categoryIndex;
    int playlistIndex;

public slots:
    void cancelClicked();
    void okClicked();

private:
    Ui::addPermalinkDialog *ui;
};

#endif // ADDPERMALINKDIALOG_H

/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef ADDDIVIDERDIALOG_H
#define ADDDIVIDERDIALOG_H

#include <QDialog>
#include "library.h"
#include "category.h"
#include "playlist.h"

namespace Ui {
class addDividerDialog;
}

class addDividerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addDividerDialog(QWidget *parent = 0, library &Library =library(), bool edit = false, int catEditIndex=-1,int playlistEditIndex=-1);
    ~addDividerDialog();


    library &libraryNew;
    bool editDialog;
    int categoryIndex;
    int playlistIndex;

public slots:
    void cancelClicked();
    void okClicked();

private:
    Ui::addDividerDialog *ui;
};

#endif // ADDDIVIDERDIALOG_H

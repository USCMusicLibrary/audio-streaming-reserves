/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef COURSEDETAILSDIALOG_H
#define COURSEDETAILSDIALOG_H

#include <QDialog>
#include "mainwindow.h"
#include "category.h"
#include "playlist.h"

namespace Ui {
class courseDetailsDialog;
}

class courseDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit courseDetailsDialog(QWidget *parent = 0, library &Library =library(), bool edit = false, int catEditIndex=-1,int playlistEditIndex=-1);
    ~courseDetailsDialog();

    library &libraryNew;
    bool editDialog;
    int categoryIndex;
    int playlistIndex;

public slots:
    void cancelClicked();
    void okClicked();

private:
    Ui::courseDetailsDialog *ui;
};

#endif // COURSEDETAILSDIALOG_H

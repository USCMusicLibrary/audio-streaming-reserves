/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QUrl>
#include "library.h"
#include "category.h"
#include "playlist.h"

namespace Ui {
class previewDialog;
}

class previewDialog : public QDialog
{
    Q_OBJECT
public slots:
    void openUrl(QUrl url);
    void doPrint();
    void openInBrowser();


public:
    explicit previewDialog(QWidget *parent = 0, const category cat = category(), library &Library =library());
    ~previewDialog();

private:
    Ui::previewDialog *ui;
};

#endif // PREVIEWDIALOG_H

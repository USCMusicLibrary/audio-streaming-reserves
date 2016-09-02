/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "previewdialog.h"
#include "ui_previewdialog.h"
#include "mainwindow.h"
#include <QWebPage>
#include <QWebFrame>
#include <QDesktopServices>
#include <QPrintDialog>
#include <QPrinter>
#include <QFile>

previewDialog::previewDialog(QWidget *parent, const category cat, library &Library) :
    QDialog(parent),
    ui(new Ui::previewDialog)
{
    ui->setupUi(this);
    std::string html = "<h1>"+cat.categoryDisplayName.toStdString()+"  (preview)</h1>";

    std::stringstream stream;
    stream<<html;
    for (playlist currentPlaylist : cat.playlists){
        if (currentPlaylist.type == "playlist"){
            stream<< "<p><strong><big>"<<currentPlaylist.playlistDisplayName.toStdString()<<"</big></strong>"
                    "<p>"<<currentPlaylist.description.toStdString()<<"</p>";

            //table headers
            stream<< "<small><pre><table style=\"border: 1px solid black;\"><tr><th>Track title</th><tr>";
            for (QString trackID : currentPlaylist.playlistTracks){
                bool found = false;
                int j=0;
                QString trackDisplayName;
                for (audioFileData track : Library.tracks){
                    if (trackID == track.filename){
                        trackDisplayName = track.displayTitle;
                        found = true;
                        break;
                    }
                    j++;
                }
                if (!found) {
                    currentPlaylist.remove(j); //TODO: error say item has been removed;
                }
                else {
                    stream<<"<tr><td>"<< MainWindow::escapeForPHP(trackDisplayName.toStdString())<<
                            "</td></tr>";
                }

            }
            stream<<"</table></pre></small>";
        }
        else if (currentPlaylist.type == "link"){
            stream<<"<p><strong><big>"<<currentPlaylist.playlistDisplayName.toStdString()<<"</big></strong>"<<
                    currentPlaylist.description.toStdString()<<"--<a target=\"_blank\" href=\"http://pallas2.tcl.sc.edu/login?url="<<currentPlaylist.permalink.toStdString()<<"\">Click here to access</a></p>";
        }
        else {
            if (currentPlaylist.displayDividerTitle){
                stream<<"<h2>"<<currentPlaylist.playlistDisplayName.toStdString()<<"</h2>";
            }
            if (currentPlaylist.displayDividerHR){
                stream<<"<hr>";
            }
        }

        //stream<<"<h2>" << currentPlaylist.playlistDisplayName.toStdString() << "</h2>";
    }
    QString htm = stream.str().c_str();
    //this->ui->textBrowser->setHtml(htm);
    //this->ui->textBrowser->setOpenExternalLinks(true);
    this->ui->webView->setHtml(htm);
    this->ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect (this->ui->webView,&QWebView::linkClicked,this,&previewDialog::openUrl);
    connect (this->ui->btnPrint,&QPushButton::clicked,this,&previewDialog::doPrint);
    connect (this->ui->btnOpenBrowser,&QPushButton::clicked,this,&previewDialog::openInBrowser);
}

void previewDialog::openInBrowser(){
    QFile file(QApplication::applicationDirPath()+"/tmphtml.html");

    file.open(QIODevice::WriteOnly);
    file.write(this->ui->webView->page()->mainFrame()->toHtml().toStdString().c_str());
    file.close();
    QDesktopServices::openUrl("file:///"+QApplication::applicationDirPath()+"/tmphtml.html");
}

void previewDialog::doPrint(){
    QPrinter printer;
        QPrintDialog printDlg(&printer);
        if (printDlg.exec() == QDialog::Rejected)
            return;

        this->ui->webView->print(&printer);
}

void previewDialog::openUrl(QUrl url){
    QDesktopServices::openUrl(url);
}


previewDialog::~previewDialog()
{
    delete ui;
}

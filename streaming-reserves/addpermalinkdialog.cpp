/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "addpermalinkdialog.h"
#include "ui_addpermalinkdialog.h"

addPermalinkDialog::addPermalinkDialog(QWidget *parent, library &Library, bool edit, int catEditIndex, int playlistEditIndex) :
    QDialog(parent),
    libraryNew(Library),
    ui(new Ui::addPermalinkDialog)
{
    editDialog = edit;
    categoryIndex = catEditIndex;
    playlistIndex = playlistEditIndex;
    ui->setupUi(this);
    connect (this->ui->btnCancel,&QPushButton::clicked,this,&addPermalinkDialog::cancelClicked);
    connect (this->ui->btnOk, &QPushButton::clicked, this, &addPermalinkDialog::okClicked);

    this->ui->groupBox->setTitle("Edit permalink details");

    if (editDialog){

        if (playlistIndex != -1){ //edit playlist instead of category
            playlist tmpPlaylist = libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex);
            this->ui->lineEditTitle->setText(tmpPlaylist.playlistDisplayName);
            this->ui->textEditDescription->setPlainText(tmpPlaylist.description);
            this->ui->lineEditPermalink->setText(tmpPlaylist.permalink);

        }
        else {
            category tmpCat = libraryNew.categories.at(categoryIndex);
            this->ui->lineEditTitle->setText(tmpCat.categoryDisplayName);
            this->ui->textEditDescription->setPlainText(tmpCat.description);
        }
    }
}

addPermalinkDialog::~addPermalinkDialog()
{
    delete ui;
}


void addPermalinkDialog::cancelClicked(){
    this->done(QDialog::Rejected);
}

void addPermalinkDialog::okClicked(){

    QString title = this->ui->lineEditTitle->text();
    QString description = this->ui->textEditDescription->toPlainText();
    QString permalink = this->ui->lineEditPermalink->text();

    category newCategory;
    playlist newPlaylist;


    if (title!= ""){
        newCategory.categoryDisplayName = title;
        newPlaylist.playlistDisplayName = title;
    }
    if (description != ""){
        newCategory.description = description;
        newPlaylist.description = description;
    }

    if (permalink!=""){
        newPlaylist.permalink = permalink;
    }
    newPlaylist.type = "link";

    if (editDialog){
        if (playlistIndex != -1){//edit playlist
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).playlistDisplayName = newPlaylist.playlistDisplayName;
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).description = newPlaylist.description;
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).permalink = newPlaylist.permalink;
        }
        else {
            libraryNew.categories.at(categoryIndex).categoryDisplayName = newCategory.categoryDisplayName;
            libraryNew.categories.at(categoryIndex).description = newCategory.description;
        }
    }
    else {
        if (playlistIndex == -1 && categoryIndex != -1){//new playlist
            libraryNew.categories.at(categoryIndex).playlists.push_back(newPlaylist);
        }
        else {
            libraryNew.categories.push_back(newCategory);
        }
    }

    this->done(QDialog::Accepted);
}

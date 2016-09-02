/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "adddividerdialog.h"
#include "ui_adddividerdialog.h"

addDividerDialog::addDividerDialog(QWidget *parent, library &Library, bool edit, int catEditIndex,int playlistEditIndex) :
    QDialog(parent),
    libraryNew(Library),
    ui(new Ui::addDividerDialog)
{
    editDialog = edit;
    categoryIndex = catEditIndex;
    playlistIndex = playlistEditIndex;
    ui->setupUi(this);
    connect (this->ui->btnCancel,&QPushButton::clicked,this,&addDividerDialog::cancelClicked);
    connect (this->ui->btnOk, &QPushButton::clicked, this, &addDividerDialog::okClicked);


    if (editDialog){

        if (playlistIndex != -1){ //edit playlist instead of category
            playlist tmpPlaylist = libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex);
            this->ui->lineEditTitle->setText(tmpPlaylist.playlistDisplayName);
            this->ui->chkDisplayTitle->setChecked(tmpPlaylist.displayDividerTitle);
            this->ui->chkDisplayHR->setChecked(tmpPlaylist.displayDividerHR);
            //this->ui->textEditDescription->setPlainText(tmpPlaylist.description);
            //this->ui->lineEditPermalink->setText(tmpPlaylist.permalink);

        }
        else {
            //category tmpCat = libraryNew.categories.at(categoryIndex);
            //this->ui->lineEditTitle->setText(tmpCat.categoryDisplayName);
            //this->ui->textEditDescription->setPlainText(tmpCat.description);

            //TODO add error code here
        }
    }


}

addDividerDialog::~addDividerDialog()
{
    delete ui;
}


void addDividerDialog::cancelClicked(){
    this->done(QDialog::Rejected);
}

void addDividerDialog::okClicked(){

    QString title = this->ui->lineEditTitle->text();
    //QString description = this->ui->textEditDescription->toPlainText();
    //QString permalink = this->ui->lineEditPermalink->text();

    //category newCategory;
    playlist newPlaylist;


    if (title!= ""){
        //newCategory.categoryDisplayName = title;
        newPlaylist.playlistDisplayName = title;
    }

    newPlaylist.type = "divider";
    newPlaylist.displayDividerHR = this->ui->chkDisplayHR->isChecked();
    newPlaylist.displayDividerTitle= this->ui->chkDisplayTitle->isChecked();

    if (editDialog){
        if (playlistIndex != -1){//edit playlist
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).playlistDisplayName = newPlaylist.playlistDisplayName;
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).displayDividerTitle = this->ui->chkDisplayTitle->isChecked();
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).displayDividerHR = this->ui->chkDisplayHR->isChecked();
        }
        else {
            //libraryNew.categories.at(categoryIndex).categoryDisplayName = newCategory.categoryDisplayName;
            //libraryNew.categories.at(categoryIndex).description = newCategory.description;

            //TODO add error code here
        }
    }
    else {
        if (playlistIndex == -1 && categoryIndex != -1){//new playlist
            libraryNew.categories.at(categoryIndex).playlists.push_back(newPlaylist);
        }
        else {
            //libraryNew.categories.push_back(newCategory);

            //TODO: add error code here
        }
    }

    this->done(QDialog::Accepted);
}

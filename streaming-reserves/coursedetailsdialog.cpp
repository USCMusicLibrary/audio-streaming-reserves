/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

courseDetailsDialog::courseDetailsDialog(QWidget *parent, library &Library, bool edit, int catEditIndex, int playlistEditIndex) :
    QDialog(parent),
    ui(new Ui::courseDetailsDialog),
    libraryNew(Library)
{
    editDialog = edit;
    categoryIndex = catEditIndex;
    playlistIndex = playlistEditIndex;
    ui->setupUi(this);
    connect (this->ui->btnCancel,&QPushButton::clicked,this,&courseDetailsDialog::cancelClicked);
    connect (this->ui->btnOK, &QPushButton::clicked, this, &courseDetailsDialog::okClicked);

    if (editDialog){

        if (playlistIndex != -1){ //edit playlist instead of category
            playlist tmpPlaylist = libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex);
            this->ui->lineEditCourseTitle->setText(tmpPlaylist.playlistDisplayName);
            this->ui->textEditCourseDescription->setPlainText(tmpPlaylist.description);
            this->ui->groupBox->setTitle("Edit playlist details");
        }
        else {
            category tmpCat = libraryNew.categories.at(categoryIndex);
            this->ui->lineEditCourseTitle->setText(tmpCat.categoryDisplayName);
            this->ui->textEditCourseDescription->setPlainText(tmpCat.description);
        }
    }
    else if (!edit && playlistEditIndex == -1){ //new playlist
        this->ui->groupBox->setTitle("Edit playlist details");
    }
}

courseDetailsDialog::~courseDetailsDialog()
{
    //libraryNew = NULL;
    delete ui;
}

void courseDetailsDialog::cancelClicked(){
    this->done(QDialog::Rejected);
}

void courseDetailsDialog::okClicked(){

    QString title = this->ui->lineEditCourseTitle->text();
    QString description = this->ui->textEditCourseDescription->toPlainText();

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

    if (editDialog){
        if (playlistIndex != -1){//edit playlist
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).playlistDisplayName = newPlaylist.playlistDisplayName;
             libraryNew.categories.at(categoryIndex).playlists.at(playlistIndex).description = newPlaylist.description;
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

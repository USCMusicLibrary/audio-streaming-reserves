/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */


/* @file mainwindow.cpp
 * MainWindow class - main program logic and interface
 * see individual functions for more details
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiofiledata.h"
#include "library.h"
#include <fstream>
#include <regex>
#include <QTime>
#include <QItemSelection>

/*
**********TODO***************

add move up/down buttons for playlists
add duplicate button for category
add filename and proxy prefix settings in mainwindow

*/

/* MainWindow constructor
 * builds main interface from mainwindow.ui and connects
 *   appropriate slots and signals
 * also adds listener for library file changes
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //rand seed is used for the lockfile, when another user is writing to
    //  library file and we need to wait a certain amount of time before
    //  trying again
    int seed;
    QTime time = QTime();
    seed = time.msecsSinceStartOfDay();
    qsrand(seed);

    //set up lockfile, to prevent other users from trying to modify at the same time
    lockFile = new QLockFile(QApplication::applicationDirPath()+"/lockfile.lock");


    //setup main ui
    ui->setupUi(this);

    this->ui->tableView->setAcceptDrops(true);
    connect(this->ui->btnAddFiles,&QPushButton::clicked,this,&MainWindow::btnAddFilesClick);

    audioFile = new QAudioDecoder(this);
    libraryPath = QApplication::applicationDirPath() + "/library";

    //try loading library file, on failure exit application
    try{
        mainLibrary  = library("library.js",this);//read library file and load into memory
    }
    catch (QString e){
        QMessageBox::about(this,"Error",e+" Application will now close.");
        exit(1);
    }

    //set up pointers for tableView and tableModel objects
    //need to use pointers because these get created and deleted
    //  during execution
    tableModel = nullptr;
    tableSortModel = nullptr;
    tableSelectionModel = nullptr;

    refreshWidgets();

    //styling for delete buttons
    ui->btnDeleteCategory->setStyleSheet("background-color: red;");
    ui->btnDeletePlaylist->setStyleSheet("background-color: red;");

    //file system watcher to receive notifications when library file is modified by another user
    this->fileWatcher = new QFileSystemWatcher(this);
    this->fileWatcher->addPath(QApplication::applicationDirPath() + "/library.js");

    connect (this->fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::updateLibrary);

    //other signals and slots
    connect (this->ui->btnUpdateTrackData,&QPushButton::clicked,this,&MainWindow::updateTrackData);
    connect (this->ui->btnAddToPlaylist, &QPushButton::clicked,this,&MainWindow::addTrackToPlaylist);

    //drag drop stuff

    connect (this->ui->btnRemoveFromPlaylist, &QPushButton::clicked,this,&MainWindow::removeFromPlaylist);
    connect (this->ui->btnMoveDownInPlaylist, &QPushButton::clicked,this,&MainWindow::moveDownInPlaylist);
    connect (this->ui->btnMoveUpInPlaylist, &QPushButton::clicked,this,&MainWindow::moveUpInPlaylist);
    connect (this->ui->btnMovePlaylistUp, &QPushButton::clicked,this,&MainWindow::movePlaylistUp);
    connect (this->ui->btnMovePlaylistDown, &QPushButton::clicked,this,&MainWindow::movePlaylistDown);

    connect (this->ui->btnGeneratePHP, &QPushButton::clicked,this,&MainWindow::generatePHP);
    connect (this->ui->btnPreviewCategory, &QPushButton::clicked,this,&MainWindow::previewCategory);

    connect (this->ui->btnRemoveFromLibrary, &QPushButton::clicked,this,&MainWindow::deleteFromLibrary);
    //connect(this->ui->TracksListWidget,&QListWidget::currentRowChanged,this,&MainWindow::updateTrackDetails);//new
    connect (this->ui->playlistsWidget,&QListWidget::currentRowChanged,this,&MainWindow::updatePlaylistItemsWidget);
    connect (this->ui->categoriesWidget,&QListWidget::currentRowChanged,this,&MainWindow::updatePlaylists);
    connect (this->ui->playlistItemsWidget,&QListWidget::currentRowChanged,this,&MainWindow::syncTrackDetails);

    //connect (this->ui->tableView, &QTableView::selectionChanged);

    connect (this->ui->btnNewPlaylist, &QPushButton::clicked,this,&MainWindow::addNewPlaylist);
    connect (this->ui->btnAddPermalink, &QPushButton::clicked,this,&MainWindow::addPermalink);
    connect (this->ui->btnAddDivider, &QPushButton::clicked, this, &MainWindow::addDivider);
    connect (this->ui->btnEditPlaylist, &QPushButton::clicked,this,&MainWindow::editPlaylist);
    connect (this->ui->btnDeletePlaylist, &QPushButton::clicked,this,&MainWindow::deletePlaylist);
    connect (this->ui->playlistsWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::editPlaylist);

    connect (this->ui->btnNewCategory, &QPushButton::clicked,this,&MainWindow::addNewCategory);
    connect (this->ui->btnEditCategory, &QPushButton::clicked, this,&MainWindow::editCategory);
    connect (this->ui->btnDeleteCategory, &QPushButton::clicked,this,&MainWindow::deleteCategory);
    connect (this->ui->btnDuplicateCategory, &QPushButton::clicked,this,&MainWindow::duplicateCategory);


    //this->ui->TracksListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->ui->playlistsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    if (mainLibrary.categories.size()>0){
        ui->categoriesWidget->setCurrentRow(0);
    }

    //need to use an event filter for tableView because we need to get notified when users drop
    //  files into the table view
    this->ui->tableView->installEventFilter(this);

}

/* @function eventFilter from QObject
 * receives all events from tableView and performs appropriate action if the event
 *   is a drag/drop event. Otherwise forwards event to default handler
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    //QMessageBox::about(this,"debug","event filter");

    if (event->type() == QEvent::DragEnter)
       {
          QDragEnterEvent *tDragEnterEvent = static_cast<QDragEnterEvent *>(event);
          tDragEnterEvent->acceptProposedAction();

          return true;
       }
       else if (event->type() == QEvent::DragMove)
       {
          QDragMoveEvent *tDragMoveEvent = static_cast<QDragMoveEvent *>(event);
          tDragMoveEvent->acceptProposedAction();

          return true;
       }
       else if (event->type() == QEvent::Drop)
       {
          QDropEvent *tDropEvent = static_cast<QDropEvent *>(event);
          tDropEvent->acceptProposedAction();

          const QMimeData *mimeData = tDropEvent->mimeData();
          QStringList fileList;
              if (mimeData->hasUrls()) {
                  QList<QUrl> urlList = mimeData->urls();
                  QString text;
                  for (int i = 0; i < urlList.size() && i < 32; ++i) {
                      QString url = urlList.at(i).toLocalFile();
                      text += url + QString("\n");
                      if (QFileInfo(url).suffix()!="mp3"){
                          QMessageBox::about(this,"Error","Invalid file(s). Please only select files with an 'mp3' extension");
                          return true;
                      }
                      fileList.push_back(url);
                  }
                  QMessageBox::StandardButton reply;
                  reply = QMessageBox::question(this, "Add files", "Add the following files to library?\n\n"+text,
                                                  QMessageBox::Yes|QMessageBox::No);
                  if (reply == QMessageBox::Yes) {
                      addTracksToLibrary(fileList);
                  }
              }
          return true;
       }
       else
       {
           // standard event processing
           return QObject::eventFilter(obj, event);
       }
}

/* @function addTracksToLibrary
 * receives a QStringList of (mp3) file paths and adds to library
 */
void MainWindow::addTracksToLibrary(QStringList files){
    if (files.isEmpty()){ //check if user selected files
        return;
    }
    //this->savePlaylists();
    //this->updatePlaylistTitle();
    if (!renameCopyFiles(files)){
        QMessageBox::about(this,"debug","renameCopyFiles failed");
    }
    oldFilesList = files;
    if (!readMP3Tags(files)){
        QMessageBox::about(this,"debug","readMP3Tags failed");
    }
    saveLibrary();
    refreshWidgets();

    setTableRow(this->mainLibrary.tracks.size()-1);
    //ui->categoriesWidget->setCurrentRow(indexCategory);
    //ui->playlistsWidget->setCurrentRow(indexPlaylist);
}

/* @function updateLibrary
 * loads library data from library file into memory
 */
void MainWindow::updateLibrary(){
    fileBeingEdited = fileBeingEdited? false : true;
    if (fileBeingEdited){
        return;
    }
    else {

        //wait a random amount of time between 0 and 500 msecs

        //int random = (qrand() % 500)+500;


lockFile->setStaleLockTime(1000);
        if (!lockFile->tryLock(2000)){
            //QMessageBox::about(this,"debug","Uhe application.");
            lockFile->unlock();
        }
        else {
    mainLibrary.updateLibrary(); //library class
    refreshWidgets();
    //QMessageBox::about(this, "debug", "updateLibrary");
    fileBeingEdited  = true;
    lockFile->unlock();
       }
    }

}

/* @function saveLibrary
 * writes changes to library file
 */
void MainWindow::saveLibrary(){
    try{
        lockFile->setStaleLockTime(1000);
        if (!lockFile->tryLock(3000)){
            //QMessageBox::about(this,"debug","Unable to secure lock file. Make sure there are no other users running the application.");
            lockFile->unlock();
        }
        QString tmpString,tmpString2;
        tmpString = QApplication::applicationDirPath() + "/library.js";
        tmpString2 = QApplication::applicationDirPath() + "/library.js.bak";
        if (!QFile::remove(tmpString2)){
            QMessageBox::about(this,"debug","failed to backupDB (remove)");
        }
        if (!QFile::copy(tmpString,tmpString2)){
            QMessageBox::about(this,"debug","failed to backupDB (copy)");
        }
        mainLibrary.saveJsonFile(errorString);
        lockFile->unlock();
    }
    catch (...){
        QMessageBox::about(this,"Error","Error saving library. Application will now close.");
        exit(1);
    }
}

/* destructor
 */
MainWindow::~MainWindow()
{

    saveLibrary();

    delete lockFile;

    delete ui;
}

/* @function syncTrackDetails
 * updates sidebar with track info when user selects a track in a playlist
 */
void MainWindow::syncTrackDetails(){
    //QMessageBox::about(this,"Debug","in syncTrackDetails");

    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    int indexItem = this->ui->playlistItemsWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1 || indexItem ==-1) return;

    playlist &currentPlaylist = mainLibrary.categories.at(indexCategory).playlists.at(indexPlaylist);


    QString trackID = currentPlaylist.playlistTracks.at(ui->playlistItemsWidget->currentRow());

    //QString trackDisplayName;
    unsigned int i = 0;
    bool found =false;
    for (audioFileData track : mainLibrary.tracks){
        if (trackID == track.filename){
            //trackDisplayName = track.displayTitle;
            found = true;
            break;
        }
        i++;
    }
    if (found) {
        setTableRow(i); //TODO: error say item has been removed;
    }
}

/* @function refreshWidgets
 * this is the main function for updating the ui
 * Used throughout whenever a change is made to the library in mermory
 */
void MainWindow::refreshWidgets(){

    ui->btnAddToPlaylist->setEnabled(false);
    ui->btnEditPlaylist->setEnabled(false);
    ui->btnMoveDownInPlaylist->setEnabled(false);
    ui->btnMoveUpInPlaylist->setEnabled(false);
    ui->btnRemoveFromPlaylist->setEnabled(false);
    ui->btnDeletePlaylist->setEnabled(false);
    ui->btnNewPlaylist->setEnabled(false);
    ui->btnAddPermalink->setEnabled(false);
    ui->btnEditCategory->setEnabled(false);
    ui->btnDeleteCategory->setEnabled(false);
    ui->btnAddToPlaylist->setEnabled(false);

    //int trackIndex = ui->TracksListWidget->currentRow();
    //int trackIndex = tableSelectionModel->currentIndex().row();
    int categoryIndex = ui->categoriesWidget->currentRow();
    int playlistIndex = ui->playlistsWidget->currentRow();
    int playlistItemIndex = ui->playlistItemsWidget->currentRow();

    //sanity check
    //trackIndex = trackIndex>=0 ? trackIndex : 0;
    categoryIndex = categoryIndex>=0 ? categoryIndex : 0;
    playlistIndex = playlistIndex>=0 ? playlistIndex : 0;
    playlistItemIndex = playlistItemIndex>=0 ? playlistItemIndex : 0;


    updateTableModel();

    //if (trackIndex > tableModel->rowCount()){
     //   trackIndex=0;
    //}
    //this->ui->tableView->setSelection(QRect(0,0,0,0),QItemSelectionModel::Rows);
    /*refresh main tracks widget

    this->ui->TracksListWidget->clear();
    this->ui->playlistsWidget->clear();
    this->ui->playlistItemsWidget->clear();

    for (audioFileData track : mainLibrary.tracks){
        QString tmpString = track.title;
        tmpString.append(" -- " + track.artist);
        tmpString.append(" -- " + track.album);
        this->ui->TracksListWidget->addItem(tmpString);
    }
    ui->TracksListWidget->setCurrentRow(trackIndex);*/


   //updateTrackDetails();

    this->ui->categoriesWidget->clear();
    for (category currentCategory : mainLibrary.categories){
        this->ui->categoriesWidget->addItem(currentCategory.categoryDisplayName);
    }

    //if (categoryIndex<=0 && categoryIndex < mainLibrary.categories.size()){
        ui->categoriesWidget->setCurrentRow(categoryIndex);
    //}

    updatePlaylists();
    //if (playlistIndex<=0 && playlistIndex < mainLibrary.categories.at(categoryIndex).playlists.size()){
        ui->playlistsWidget->setCurrentRow(playlistIndex);
    //}

    updatePlaylistItemsWidget();
    //if (playlistItemIndex<=0 && playlistItemIndex < mainLibrary.categories.at(categoryIndex).playlists.at(playlistIndex).playlistTracks.size()){
        ui->playlistItemsWidget->setCurrentRow(playlistItemIndex);
    //}

    //saveLibrary();

}


int MainWindow::savePlaylists(){
    return 0;
}

/* @function deleteFromLibrary
 * removes references to track from library and deletes mp3 files from disk
 */
void MainWindow::deleteFromLibrary(){
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1){
        indexPlaylist = 0;
        indexCategory = 0;
    }


    if (QMessageBox::question(this,"Delete files from library.","Please make sure that no playlists currently use these files. Do you wish to continue to delete these files?",QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)return;
    //int currentItem = ui->TracksListWidget->currentRow();

    QModelIndexList indexList = tableSelectionModel->selectedRows();

    int S = indexList.size();
    if (S<1) return;

    std::vector<int> indices;
    for(int i = 0; i < S; i++)  indices.push_back(indexList.at(i).row());

    QStringList files;
    int s = indices.size()-1;
    while (s>=0){
        files.push_back(mainLibrary.tracks.at(indices.at(s)).filename);
        s = s-1;
    }
    s = indices.size()-1;
    std::sort (indices.begin(), indices.end() );
    while (s>=0){
        mainLibrary.tracks.erase(mainLibrary.tracks.begin()+indices.at(s));
        s = s-1;
    }

    /*QList<QListWidgetItem*> selection = this->ui->TracksListWidget->selectedItems();
    int S = selection.size();
    std::vector<int> indices;
    for(int i = 0; i < S; i++)  indices.push_back(this->ui->TracksListWidget->row(selection[i]));

    QStringList files;
    int s = indices.size()-1;
    while (s>=0){
        files.push_back(mainLibrary.tracks.at(indices.at(s)).filename);
        //files.push_back(TrackDB.at(indices.at(s)).at(5));
        //mainLibrary.tracks.erase(mainLibrary.tracks.begin()+indices.at(s));
        //TrackDB.erase(TrackDB.begin()+indices.at(s));
        s = s-1;
    }
    s = indices.size()-1;
    std::sort (indices.begin(), indices.end() );
    while (s>=0){
        //files.push_back(mainLibrary.tracks.at(indices.at(s)).filename);
        //files.push_back(TrackDB.at(indices.at(s)).at(5));
        mainLibrary.tracks.erase(mainLibrary.tracks.begin()+indices.at(s));
        //TrackDB.erase(TrackDB.begin()+indices.at(s));
        s = s-1;
    }

    ui->TracksListWidget->clear();*/

    saveLibrary();
    refreshWidgets();
    setTableRow(0);
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist);


    QString file;
    for (unsigned int i=0; i<indices.size(); i++){
        file = QApplication::applicationDirPath() + "/library/" + files.at(i);
        QFile::remove(file);
    }

}

/* @function setTableRow
 * custom function to select a row in the tableView
 */
void MainWindow::setTableRow(int row){
    //QMessageBox::about(this,"debug","setTableRow");
    //QModelIndex index = this->ui->tableView->model()->index(row,0);
    //this->ui->tableView->selectionModel()->select( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows|QItemSelectionModel::Current );
    if (row<0 || row >= tableModel->rowCount()){

        //this->ui->tableView->model()->selectRow(0);
        return;
    }
    QModelIndex index = this->tableModel->index(row,0);

    tableSelectionModel->select(index, QItemSelectionModel::ClearAndSelect);
    //this->ui->tableView->scrollTo(index,QAbstractItemView::EnsureVisible);
    this->ui->tableView->selectRow(tableSortModel->mapFromSource(index).row());
}

/* @function updateTrackData
 * saves changes made in track details sidebar
 */
void MainWindow::updateTrackData(){
    //int index =  this->ui->TracksListWidget->currentRow();
    int index = tableSelectionModel->currentIndex().row();

    int categoryIndex = ui->categoriesWidget->currentRow();
    int playlistIndex = ui->playlistsWidget->currentRow();
    int playlistItemIndex = ui->playlistItemsWidget->currentRow();

    QString tmpString;// = this->ui->trkDisplayTitle->text();

    if (index==-1) return; //check for valid index

    QString tmpTitle = this->ui->txtEditTitle->toPlainText();
    tmpTitle.replace("\n"," ");
    mainLibrary.tracks.at(index).title = tmpTitle;

    QString tmpComposer = this->ui->txtEditComposer->toPlainText();
    tmpComposer.replace("\n"," ");
    mainLibrary.tracks.at(index).composer = tmpComposer;

    QString tmpArtist = this->ui->txtEditArtist->toPlainText();
    tmpArtist.replace("\n"," ");
    mainLibrary.tracks.at(index).artist = tmpArtist;

    QString tmpAlbum = this->ui->txtEditAlbum->toPlainText();
    tmpAlbum.replace("\n"," ");
    mainLibrary.tracks.at(index).album = tmpAlbum;

    QString tmpTrack = this->ui->lineEditTrack->text();
    mainLibrary.tracks.at(index).track = tmpTrack;

    QString tmpYear = this->ui->lineEditYear->text();
    mainLibrary.tracks.at(index).year = tmpYear;

    QString tmpPlaylistTitle = this->ui->trkDisplayTitle->text();
    mainLibrary.tracks.at(index).displayTitle = tmpPlaylistTitle;

    QString tmpCdNumber = this->ui->trkCdNumber->text();
    mainLibrary.tracks.at(index).cdNumber = tmpCdNumber;

    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(categoryIndex);
    ui->playlistsWidget->setCurrentRow( playlistIndex);
    ui->playlistItemsWidget->setCurrentRow(playlistItemIndex);

    setTableRow(index);



}

//deprecated
bool MainWindow::readDBFile(){
    return true;
}

/* @function updateTableModel
 * deletes old table model structures and creates a new one
 */
void MainWindow::updateTableModel(){
    if (tableModel != nullptr) delete tableModel;
    if (tableSortModel != nullptr) delete tableSortModel;
    if (tableSelectionModel != nullptr) delete tableSelectionModel;

    tableModel = new tracksTableModel();
    tableModel->setLibraryPointer(&mainLibrary);
    tableSortModel = new QSortFilterProxyModel(this);
    tableSortModel->setSourceModel(tableModel);
    this->ui->tableView->setModel(tableSortModel);
    this->tableSelectionModel = this->ui->tableView->selectionModel();
    connect (this->tableSelectionModel,&QItemSelectionModel::selectionChanged,this, &MainWindow::updateTrackDetails);
}

/* @function updateTrackDetails
 * updates details sidebar with info on selected track
 */
void MainWindow::updateTrackDetails(const QItemSelection &selected, const QItemSelection &deselected){

    QModelIndexList indexList = selected.indexes();
    if (indexList.size()<1){
        return;
    }

    QModelIndex currentIndex = indexList.at(0);

    QModelIndex newIndex = tableSortModel->index(currentIndex.row(),5);

    QString filename = this->tableSortModel->data(newIndex,Qt::DisplayRole).toString();

    //QString trackDisplayName;
    unsigned int i = 0;
    bool found =false;
    int index;
    for (audioFileData track : mainLibrary.tracks){
        if (filename == track.filename){
            //trackDisplayName = track.displayTitle;
            found = true;
            break;
        }
        i++;
    }
    if (found) {
        index = i;
    }

    //QMessageBox::about(this,"debug","updateTrackDetails");
    /*update track details in left hand pane

    int index = this->ui->TracksListWidget->currentRow();
    //TODO fix this error reporting
    if (index==-1) return;*/

    this->ui->txtEditTitle->setPlainText(mainLibrary.tracks.at(index).title);
    this->ui->txtEditComposer->setPlainText(mainLibrary.tracks.at(index).composer);
    this->ui->txtEditArtist->setPlainText(mainLibrary.tracks.at(index).artist);
    this->ui->txtEditAlbum->setPlainText(mainLibrary.tracks.at(index).album);
    this->ui->lineEditTrack->setText(mainLibrary.tracks.at(index).track);
    this->ui->lineEditYear->setText(mainLibrary.tracks.at(index).year);
    this->ui->trkFilename->setText("File name: "+mainLibrary.tracks.at(index).filename);
    this->ui->trkOriginalFilename->setText("Original file name: "+mainLibrary.tracks.at(index).originalFilename);
    this->ui->trkFormat->setText("Format: "+mainLibrary.tracks.at(index).format);
    this->ui->trkDisplayTitle->setText(mainLibrary.tracks.at(index).displayTitle);
    this->ui->trkCdNumber->setText(mainLibrary.tracks.at(index).cdNumber);
}

/* @function updatePlaylists
 * updates playlist view
 */
void MainWindow::updatePlaylists(){
    if (mainLibrary.categories.size()>0){
    ui->btnEditCategory->setEnabled(true);
    ui->btnDeleteCategory->setEnabled(true);
    ui->btnNewPlaylist->setEnabled(true);
    ui->btnAddPermalink->setEnabled(true);
    }


    this->ui->playlistsWidget->clear();
    this->ui->playlistItemsWidget->clear();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexCategory==-1) return;

    category currentCategory = mainLibrary.categories.at(indexCategory);

    for (playlist currentPlaylist : currentCategory.playlists){
        if (currentPlaylist.type == "link"){
            this->ui->playlistsWidget->addItem("[link] " + currentPlaylist.playlistDisplayName);
        }
        else if (currentPlaylist.type == "playlist") { //type=="playlist"
            this->ui->playlistsWidget->addItem(currentPlaylist.playlistDisplayName);
        }
        else {
            this->ui->playlistsWidget->addItem("[divider] " + currentPlaylist.playlistDisplayName);
        }
    }

    ui->playlistsWidget->setCurrentRow(0);

}

/* @functionAddNewCategory
 * shows dialog to add new category
 */
void MainWindow::addNewCategory(){
    courseDetailsDialog dialog(this,mainLibrary);
    int currentRow = ui->categoriesWidget->currentRow();

    if (dialog.exec() == QDialog::Accepted){
        mainLibrary.sortCategories();
        saveLibrary();
        refreshWidgets();
        if (currentRow != -1){
            this->ui->categoriesWidget->setCurrentRow(mainLibrary.categories.size()-1);
        }
        QMessageBox::about(this,"debug","accepted");
    }
    else {
        QMessageBox::about(this,"debug","rejected");
    }
}

/* @function editCategory
 * shows dialog to edit category
 */
void MainWindow::editCategory(){
    int currentRow = this->ui->categoriesWidget->currentRow();
    courseDetailsDialog dialog(this,mainLibrary,true,currentRow);

    if (dialog.exec() == QDialog::Accepted){
        mainLibrary.sortCategories();
        saveLibrary();
        refreshWidgets();
        this->ui->categoriesWidget->setCurrentRow(currentRow);
        QMessageBox::about(this,"debug","accepted");
    }
    else {
        QMessageBox::about(this,"debug","rejected");
    }

}

/* @function ducplicateCategory
 */
void MainWindow::duplicateCategory(){
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexCategory==-1) return;

    category newCat = mainLibrary.categories.at(indexCategory);
    mainLibrary.categories.push_back(newCat);
    mainLibrary.sortCategories();
    saveLibrary();
    this->refreshWidgets();

}

/* @function deleteCategory
 */
void MainWindow::deleteCategory(){
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexCategory==-1) return;

    if (QMessageBox::question(this,"Delete category.","This will delete the selected category and all playlists associated with it. This cannot be undone. Do you wish to continue to delete this category?",QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)return;

    mainLibrary.categories.erase(mainLibrary.categories.begin()+indexCategory);

    saveLibrary();
    this->refreshWidgets();

}

/* @function addPermalink
 */
void MainWindow::addPermalink(){
    int catIndex = this->ui->categoriesWidget->currentRow();

    addPermalinkDialog dialog(this,mainLibrary,false,catIndex);

    if (dialog.exec() == QDialog::Accepted){
        saveLibrary();
        refreshWidgets();
        this->ui->categoriesWidget->setCurrentRow(catIndex);
        this->ui->playlistsWidget->setCurrentRow(mainLibrary.categories.at(catIndex).playlists.size()-1);
        QMessageBox::about(this,"debug","accepted");
    }
    else {
        QMessageBox::about(this,"debug","rejected");
    }
}

/* @function addDivider
 */
void MainWindow::addDivider(){
    int catIndex = this->ui->categoriesWidget->currentRow();

    addDividerDialog dialog(this,mainLibrary,false,catIndex);

    if (dialog.exec() == QDialog::Accepted){
        saveLibrary();
        refreshWidgets();
        this->ui->categoriesWidget->setCurrentRow(catIndex);
        this->ui->playlistsWidget->setCurrentRow(mainLibrary.categories.at(catIndex).playlists.size()-1);
        QMessageBox::about(this,"debug","accepted");
    }
    else {
        QMessageBox::about(this,"debug","rejected");
    }
}

/* @function addNewPlaylist
 */
void MainWindow::addNewPlaylist(){

    int catIndex = this->ui->categoriesWidget->currentRow();

    courseDetailsDialog dialog(this,mainLibrary,false,catIndex);

    if (dialog.exec() == QDialog::Accepted){
        saveLibrary();
        refreshWidgets();
        this->ui->categoriesWidget->setCurrentRow(catIndex);
        this->ui->playlistsWidget->setCurrentRow(mainLibrary.categories.at(catIndex).playlists.size()-1);
        QMessageBox::about(this,"debug","accepted");
    }
    else {
        QMessageBox::about(this,"debug","rejected");
    }

}

/* @function editPlaylist
 */
void MainWindow::editPlaylist(){
    int catIndex = this->ui->categoriesWidget->currentRow();
    int playlistIndex = this->ui->playlistsWidget->currentRow();
    courseDetailsDialog dialog(this,mainLibrary,true,catIndex,playlistIndex);

    addPermalinkDialog dialog2(this, mainLibrary,true,catIndex,playlistIndex);

    addDividerDialog dialog3 (this, mainLibrary, true,catIndex,playlistIndex);

    playlist & currentPlaylist = mainLibrary.categories.at(catIndex).playlists.at(playlistIndex);

    if (currentPlaylist.type == "playlist"){
        if (dialog.exec() == QDialog::Accepted){
            saveLibrary();
            refreshWidgets();
            this->ui->categoriesWidget->setCurrentRow(catIndex);
            this->ui->playlistsWidget->setCurrentRow(playlistIndex);
            QMessageBox::about(this,"debug","accepted");
        }
        else {
            QMessageBox::about(this,"debug","rejected");
        }
    }
    else if (currentPlaylist.type == "link"){ //permalink instead of playlist
        if (dialog2.exec() == QDialog::Accepted){
            saveLibrary();
            refreshWidgets();
            this->ui->categoriesWidget->setCurrentRow(catIndex);
            this->ui->playlistsWidget->setCurrentRow(playlistIndex);
            QMessageBox::about(this,"debug","accepted");
        }
        else {
            QMessageBox::about(this,"debug","rejected");
        }
    }
    else { //divider
        if (dialog3.exec() == QDialog::Accepted){
            saveLibrary();
            refreshWidgets();
            this->ui->categoriesWidget->setCurrentRow(catIndex);
            this->ui->playlistsWidget->setCurrentRow(playlistIndex);
            QMessageBox::about(this,"debug","accepted");
        }
        else {
            QMessageBox::about(this,"debug","rejected");
        }
    }


}

/* @function addTrackToPlaylist
 * adds a track selection to current playlist
 */
void MainWindow::addTrackToPlaylist(){
    int trackIndex = tableSelectionModel->currentIndex().row();
    if (trackIndex==-1) {
        QMessageBox::about(this,"debug","Please select a track.");
        return;
    }
    int playlistIndex = this->ui->playlistsWidget->currentRow();
    if (playlistIndex==-1) {
        QMessageBox::about(this,"debug","Please select a playlist.");
        return;
    }
    int categoryIndex = this->ui->categoriesWidget->currentRow();
    if (categoryIndex==-1){
        QMessageBox::about(this,"debug","Please select a category.");
        return;
    }
    QModelIndexList selection = tableSelectionModel->selectedRows();
    //QList<QListWidgetItem*> selection = this->ui->TracksListWidget->selectedItems();
    int S = selection.size();
    std::vector<int> indices;
    for(int i = 0; i < S; i++)  indices.push_back(tableSelectionModel->selectedRows().at(i).row());

    for (int trackIndex : indices){
        audioFileData track = mainLibrary.tracks.at(trackIndex);
        mainLibrary.categories.at(categoryIndex).playlists.at(playlistIndex).addNewTrack(track.filename);
        //categories.at(categoryIndex).playlists.at(playlistIndex).addNewTrack(TrackDB.at(indices.at(i)).at(8),TrackDB.at(indices.at(i)).at(5));
        updatePlaylistItemsWidget();
    }

    saveLibrary();
    refreshWidgets();
    //playlists.at(playlistIndex).addNewTrack(TrackDB.at(trackIndex).at(8),TrackDB.at(trackIndex).at(5));
    //this->ui->playlistItemsWidget->addItem(TrackDB.at(trackIndex).at(8));
}

/* @function updatePlaylistItemsWidget
 */
void MainWindow::updatePlaylistItemsWidget(){



    this->ui->playlistItemsWidget->clear();
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1) return;

    playlist &currentPlaylist = mainLibrary.categories.at(indexCategory).playlists.at(indexPlaylist);

    if (mainLibrary.categories.size()>0){
        int catIndex = ui->categoriesWidget->currentRow();
        if (catIndex>=0){
            if (currentPlaylist.type == "playlist"){
                ui->btnEditPlaylist->setEnabled(true);
                ui->btnDeletePlaylist->setEnabled(true);
                ui->btnAddToPlaylist->setEnabled(true);
            }
            else {
                ui->btnEditPlaylist->setEnabled(true);
                ui->btnDeletePlaylist->setEnabled(true);
                ui->btnAddToPlaylist->setEnabled(false);
            }
        }
    }

    unsigned int i =0;
    for (QString trackID : currentPlaylist.playlistTracks){
        bool found = false;

        QString trackDisplayName;
        for (audioFileData track : mainLibrary.tracks){
            if (trackID == track.filename){
                trackDisplayName = track.displayTitle;
                found = true;
                break;
            }
        }
        if (!found) {
            currentPlaylist.remove(i); //TODO: error say item has been removed;
        }
        /*while (!found){
            if (trackID == mainLibrary.tracks.at(i).filename){
                trackDisplayName = mainLibrary.tracks.at(i++).displayTitle;
                found = true;
            }
            if (i++>=mainLibrary.tracks.size())
                break; //TODO throw exception
        }*/
        else {
            this->ui->playlistItemsWidget->addItem(trackDisplayName);
            i++;
        }
    }

    if (currentPlaylist.playlistTracks.size()>0){
        ui->btnRemoveFromPlaylist->setEnabled(true);
        if (currentPlaylist.playlistTracks.size()>1){
            ui->btnMoveDownInPlaylist->setEnabled(true);
            ui->btnMoveUpInPlaylist->setEnabled(true);
        }
    }

    ui->playlistItemsWidget->setCurrentRow(0);
}

/* @function deletePlaylist
 */
void MainWindow::deletePlaylist(){
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1) return;

    if (QMessageBox::question(this,"Delete playlists.","This will delete the selected playlist(s). Do you wish to continue to delete the playlist(s)?",QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)return;


    category & currentCat =  mainLibrary.categories.at(indexCategory);


    QList<QListWidgetItem*> selection = this->ui->playlistsWidget->selectedItems();
    int S = selection.size();
    std::vector<int> indices;
    for(int i = 0; i < S; i++)  indices.push_back(this->ui->playlistsWidget->row(selection[i]));



    std::sort (indices.begin(), indices.end() );
    int s = indices.size()-1;
    while (s>=0){

        currentCat.playlists.erase(currentCat.playlists.begin()+indices.at(s));
        //mainLibrary.tracks.erase(mainLibrary.tracks.begin()+indices.at(s));

        s = s-1;
    }
    saveLibrary();
    refreshWidgets();
    this->ui->categoriesWidget->setCurrentRow(indexCategory);

}

/* @function removeFromPlaylist
 */
void MainWindow::removeFromPlaylist(){
    int indexTrack = this->ui->playlistItemsWidget->currentRow();
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1 ||indexTrack==-1) return;

    playlist & currentPlaylist = mainLibrary.categories.at(indexCategory).playlists.at(indexPlaylist);
    currentPlaylist.remove(indexTrack);
    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist);
    ui->playlistItemsWidget->setCurrentRow(indexTrack-1);

}

/* @function moveUpInPlaylist
 */
void MainWindow::moveUpInPlaylist(){
    int indexTrack = this->ui->playlistItemsWidget->currentRow();
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1 ||indexTrack==-1) return;

    playlist & currentPlaylist = mainLibrary.categories.at(indexCategory).playlists.at(indexPlaylist);

    currentPlaylist.moveUp(indexTrack);
    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist);
    ui->playlistItemsWidget->setCurrentRow(indexTrack-1);

}

/* @function moveDownInPlaylist
 */
void MainWindow::moveDownInPlaylist(){
    int indexTrack = this->ui->playlistItemsWidget->currentRow();
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1 ||indexTrack==-1) return;

    playlist & currentPlaylist = mainLibrary.categories.at(indexCategory).playlists.at(indexPlaylist);

    currentPlaylist.moveDown(indexTrack);
    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist);
    ui->playlistItemsWidget->setCurrentRow(indexTrack+1);

}

/* @function modePlaylistDown
 */
void MainWindow::movePlaylistDown(){
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1) return;

    category & currentCategory = mainLibrary.categories.at(indexCategory);

    if (indexPlaylist == currentCategory.playlists.size()-1) return;
    currentCategory.moveDown(indexPlaylist);
    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist+1);

}

/* @function movePlaylistUp
 */
void MainWindow::movePlaylistUp(){
    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1) return;

    category & currentCategory = mainLibrary.categories.at(indexCategory);

    if (indexPlaylist == 0 ) return;
    currentCategory.moveUp(indexPlaylist);
    saveLibrary();
    refreshWidgets();
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist-1);
}

void MainWindow::previewCategory(){
    int catIndex = this->ui->categoriesWidget->currentRow();
    if (catIndex<0)return;
    category cat = this->mainLibrary.categories.at(catIndex);
    previewDialog dialog(this,cat,mainLibrary);

    dialog.exec();

}

/* @function generatePHP
 * takes current selected category and generates a php file that
 * can be used to display the players and or outside links
 * uses php-template.php
 */
void MainWindow::generatePHP(){

    //TODO CHECK FOR VALID INDEX FIRST!!
    category currentCategory = mainLibrary.categories.at(ui->categoriesWidget->currentRow());

    /*QFile phpTemplate(QApplication::applicationDirPath() + "/php-template.php");
    QByteArray data;
    if (!phpTemplate.open(QIODevice::ReadOnly)){
        QString error = "Unable to open php template file.";
        throw QString("Error: "+error);
    }
    data = phpTemplate.readAll();
    phpTemplate.close();
    */

    std::ifstream phpTemplate;

    phpTemplate.open((QApplication::applicationDirPath() + "/php-template.php").toStdString());
    if (!phpTemplate.is_open()){
        return;
    }

    std::string phpString = "";
    std::string tmpString;
    while(std::getline(phpTemplate,tmpString)){
        phpString += tmpString + "\n";
    }




    //QString tmpString = data;
    //std::string phpString = tmpString.toStdString().c_str();
    std::regex e ("<\\?php\\[category-data\\]\\?>");

    std::stringstream stream;

    std::string filenamePrefix = "https://delphi.tcl.sc.edu/library/music/play/audio/";
    std::string proxyPrefix = "http://pallas2.tcl.sc.edu/login?url=";

    stream<<"<?php "<<std::endl
            <<"$filenamePrefix = \""<<filenamePrefix<<"\";"<<std::endl
            <<"$proxyPrefix = \""<<proxyPrefix<<"\";"<<std::endl
            <<"$categoryTitle = \""<<escapeForPHP(currentCategory.categoryDisplayName.toStdString())<<"\";"<<std::endl
            <<"$categoryDescription = \""<<escapeForPHP(currentCategory.description.toStdString())<<"\";"<<std::endl
            <<"$playlists = array ("<<std::endl;

    unsigned int i=0;
    for (playlist currentPlaylist : currentCategory.playlists){
        stream<<"  array("<<std::endl
            <<"  \"description\" => \""<<escapeForPHP(currentPlaylist.description.toStdString())<<"\","<<std::endl
                <<"  \"name\" => \""<<currentPlaylist.playlistID.toStdString()<<"\","<<std::endl
                <<"  \"permalink\" => \""<<currentPlaylist.permalink.toStdString()<<"\","<<std::endl
                <<"  \"title\" => \""<<escapeForPHP(currentPlaylist.playlistDisplayName.toStdString())<<"\","<<std::endl
                <<"  \"displayDividerHR\" => \""<<currentPlaylist.displayDividerHR<<"\","<<std::endl
                <<"  \"displayDividerTitle\" => \""<<currentPlaylist.displayDividerTitle<<"\","<<std::endl
                <<"  \"tracks\" => array("<<std::endl;;


        for (QString trackID : currentPlaylist.playlistTracks){
            bool found = false;
            int j=0;
            QString trackDisplayName;
            for (audioFileData track : mainLibrary.tracks){
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
                stream<<"    array(\"title\" => \""<<escapeForPHP(trackDisplayName.toStdString())<<"\", \"mp3\" => $filenamePrefix.\""<<trackID.toStdString()<<"\")";
                bool endOfArrayList = ++j==currentPlaylist.playlistTracks.size();
                if (endOfArrayList){
                    stream<< "";
                }
                else {
                    stream<< ",";
                }
                stream<<std::endl;

            }

        }
        stream<<"  ),"<<std::endl;
        stream<<"  \"type\" => \""<<currentPlaylist.type.toStdString()<<"\""<<std::endl;
        bool endOfArrayList = ++i==currentCategory.playlists.size();
        if (endOfArrayList){
            stream<< "  )";
        }
        else {
            stream<< "  ),";
        }
        stream<<std::endl;
        stream<<std::endl;
    }

    stream<<std::endl<<");"<<std::endl<<"?>"<<std::endl;

    std::string replaceString = stream.str();


    std::string newPhpString  =  std::regex_replace (phpString,e,replaceString);


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   "",
                                   tr("PHP files (*.php)"));
    //QMessageBox::about(this,"debug",fileName);
    if (fileName=="") return;
    std::ofstream phpfile;
    phpfile.open(fileName.toStdString());
    if (!phpfile.is_open()){
        QMessageBox::about(this,"debug","Unable to save PHP file.");
        return;
    }

    //e = "^$\\n";

    //newPhpString = std::regex_replace(newPhpString,e,"\n");

    phpfile<<newPhpString<<std::endl;
    phpfile.close();


}


//Escapes newlines to <br> and double quotes to \" for use in php variable
//it also escapes the \ character
std::string MainWindow::escapeForPHP(std::string &inputString){
    std::regex e("\\\\");
    std::regex e2 ("\\n");
    std::regex e3("\"");


    std::string tempString = std::regex_replace(inputString,e,"\\\\");
    tempString = std::regex_replace(tempString,e2,"<br>\n");
    tempString = std::regex_replace(tempString,e3,"\\\"");

    return  tempString;
}


/* @function btnAddFilesClick
 * adds new tracks to library
 */
void MainWindow::btnAddFilesClick(){
    //fileData = new audioFileData(this);
    //fileData->readFileInfo();

    int indexPlaylist = this->ui->playlistsWidget->currentRow();
    int indexCategory = this->ui->categoriesWidget->currentRow();
    if (indexPlaylist==-1 || indexCategory==-1){
        indexPlaylist = 0;
        indexCategory = 0;
    }

    //QMessageBox::about(this,"debug","btnBrowseClicked");
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            "",
                            "MP3 files (*.mp3)");

    addTracksToLibrary(files);

    /*
    if (files.isEmpty()){ //check if user selected files
        return;
    }
    //this->savePlaylists();
    //this->updatePlaylistTitle();
    if (!renameCopyFiles(files)){
        QMessageBox::about(this,"debug","renameCopyFiles failed");
    }
    oldFilesList = files;
    if (!readMP3Tags(files)){
        QMessageBox::about(this,"debug","readMP3Tags failed");
    }
    saveLibrary();
    refreshWidgets();

    setTableRow(0);
    ui->categoriesWidget->setCurrentRow(indexCategory);
    ui->playlistsWidget->setCurrentRow(indexPlaylist);

    */

}

bool MainWindow::addFilesToDB(){
    return true;
}

/* @function renameCopyFiles
 * takes a list of files and copies them to library directory
 * then renames according to date
 */
bool MainWindow::renameCopyFiles(QStringList files){
    newFilesList.clear();
    newFilesPathList.clear();
    QDateTime dt=QDateTime::currentDateTimeUtc();
    QString dtStr = dt.toString("yyyyMMddhhmmss");
    //QMessageBox::about(this,"debug",dtStr);
    for (int i=0; i<files.length(); i++){
        QString tmpPath = libraryPath+ "/" +dtStr + QString::number(i)+".mp3";
        QString tmpName = dtStr + QString::number(i)+".mp3";
        //QMessageBox::about(this,"debug",tmpPath);
        if (!QFile::copy(files.at(i),tmpPath)){
            errorCode = 10; //TODO define error codes!!!
            errorString = "Unable to copy files. Make sure file exists.";
            return false;
        }
        newFilesPathList.append(tmpPath);
        newFilesList.append(tmpName);
    }
    return true;
}

/* @function readMP3Tags
 * uses helper application to read MP3 tags and add to library
 * creates batchFile.bat to process mp3 tags
 */
bool MainWindow::readMP3Tags(QStringList files){
    std::ofstream batchFile;
    QString batchPath = QApplication::applicationDirPath() + "/batchFile.bat";
    batchFile.open(batchPath.toStdString());
    batchFile<<"@ECHO OFF"<<std::endl;
    batchFile<<"cd \""<<QApplication::applicationDirPath().toStdString()<<"\""<<std::endl;
    for (int i=0; i<files.length(); i++){
        batchFile<<"Tag.exe library/"<<newFilesList.at(i).toStdString()<<" > tmp"<<i<<".txt 2>&1"<<std::endl;
    }
    batchFile.close();

    processPath = batchPath;

    QStringList arguments;

    mp3tagProcess = new QProcess(this);
    mp3tagProcess->start(processPath,arguments);
    mp3tagProcess->waitForFinished(120000);
    //QMessageBox::about(this,"debug","finished batch file");

    //std::ofstream dbfile;
    //QString dbFilePath = QApplication::applicationDirPath() + "/trackDB.inf";
    //dbfile.open(dbFilePath.toStdString(),std::fstream::app);

    for (int i=0; i<files.length(); i++){
        QString tmpFilePath = QApplication::applicationDirPath() + "/tmp"+QString::number(i)+".txt";
        audioFileData newTrack;

        //audioFileData *fileData = new audioFileData(this,tmpFilePath);
        if (!newTrack.readFileInfo(tmpFilePath)){
            //QMessageBox::about(this,"debug","readFileInfo (object) failed");
            //TODO: set error codes;
            return false;
        }
        newTrack.filename = newFilesList.at(i);
        newTrack.originalFilename = oldFilesList.at(i);
        mainLibrary.tracks.push_back(newTrack);


        /*dbfile<<fileData->title.toStdString()<<";;";
        dbfile<<fileData->artist.toStdString()<<";;";
        dbfile<<fileData->album.toStdString()<<";;";
        dbfile<<fileData->track.toStdString()<<";;";
        dbfile<<fileData->year.toStdString()<<";;";
        dbfile<<newFilesList.at(i).toStdString()<<";;";
        dbfile<<oldFilesList.at(i).toStdString()<<";;";
        dbfile<<fileData->format.toStdString()<<";;";
        dbfile<<fileData->displayTitle.toStdString()<<";;"<<std::endl;*/

    }
    //dbfile.close();

    return true;
}


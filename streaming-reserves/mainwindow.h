/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtMultimedia/QAudioDecoder>
#include <QFileDialog>
#include <QDateTime>
#include <iostream>
#include <sstream>
#include <QProcess>
#include <QTableView>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLockFile>
#include <QFileSystemWatcher>
#include <QItemSelectionModel>
#include <algorithm>
#include <thread>
#include <mutex>
#include <QSortFilterProxyModel>
#include "audiofiledata.h"
#include "playlist.h"
#include "category.h"
#include "library.h"
#include "coursedetailsdialog.h"
#include "addpermalinkdialog.h"
#include "adddividerdialog.h"
#include "updater.h"
#include "tracksTableModel.h"
#include "previewdialog.h"

/* MainWindow class
 * main user interface
 * also holds main instance of library class
 */

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
static std::string escapeForPHP(std::string &inputString);

public:
    explicit MainWindow(QWidget *parent = 0);
    QString errorString;
    ~MainWindow();
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void btnAddFilesClick();


    void updateTrackDetails(const QItemSelection &selected, const QItemSelection &deselected);

    void addTracksToLibrary(QStringList files);

    void updateTrackData();
    void addNewPlaylist();
    void addTrackToPlaylist();
    void updatePlaylistItemsWidget();
    void deletePlaylist();
    void removeFromPlaylist();
    void moveUpInPlaylist();
    void moveDownInPlaylist();
    void generatePHP();
    void deleteFromLibrary();
    void updatePlaylists();
    void addNewCategory();
    void deleteCategory();

    void refreshWidgets();
    void editCategory();
    void editPlaylist();
    void addPermalink();
    void addDivider();
    void syncTrackDetails();
    void duplicateCategory();
    void movePlaylistDown();
    void movePlaylistUp();

    void saveLibrary();

    void updateLibrary();

    void previewCategory();


private:
    Ui::MainWindow *ui;
    QAudioDecoder *audioFile;
    QString libraryPath;
    QStringList newFilesList;
    QStringList newFilesPathList;
    QStringList oldFilesList;
    int errorCode;

    audioFileData *fileData;
    bool renameCopyFiles(QStringList files);
    bool addFilesToDB();
    bool readMP3Tags(QStringList files);
    QProcess * mp3tagProcess;
    QString processPath;
    bool readDBFile();
    std::vector <QStringList> TrackDB;
    std::vector <category> categories;
    std::vector <playlist> playlists;
    QSqlTableModel *model;
    std::vector <std::string> playlistFilenames;
    int savePlaylists();



    library mainLibrary;

    QLockFile *lockFile;
    QFileSystemWatcher * fileWatcher;

    int seed;

    bool fileBeingEdited;

    tracksTableModel * tableModel;
    QSortFilterProxyModel * tableSortModel;
    QItemSelectionModel * tableSelectionModel;
    void updateTableModel();
    void setTableRow(int row);


};

#endif // MAINWINDOW_H


/*
ERROR CODES:

10 = unable to copy file in renameCopyFiles

*/

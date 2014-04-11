#include "pcloudapp.h"
#include "common.h"
#include <QMenu>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <unistd.h>
#include <QDebug> //temp
#include "unistd.h" //for sync statuses
#include <QTextCodec>
#include <QWidgetAction> //temp maybe
#include <QMutex>


PCloudApp * PCloudApp::appStatic = NULL;
QMutex mutex(QMutex::Recursive);

void PCloudApp::hideAllWindows(){
    if (regwin)
        regwin->hide();
    if (logwin)
        logwin->hide();
    if (pCloudWin)
        pCloudWin->hide();
}

void PCloudApp::showWindow(QMainWindow *win)
{
    win->raise();
    win->activateWindow();
    win->showNormal();
    win->setWindowState(Qt::WindowActive);
    this->setActiveWindow(win);
}


void PCloudApp::showRegister(){
    hideAllWindows();
    if (!regwin)
        regwin=new RegisterWindow(this);
    showWindow(regwin);
}

void PCloudApp::showLogin(){
    hideAllWindows();
    if (!logwin)
        logwin=new LoginWindow(this);
    showWindow(logwin);
}
void PCloudApp::showAccount(){
    hideAllWindows();
    pCloudWin->showpcloudWindow(1);
}

/*p void PCloudApp::showShares()
{
    hideAllWindows();
    pCloudWin->showpcloudWindow(2);
}*/
void PCloudApp::showSync()
{
    hideAllWindows();
    pCloudWin->showpcloudWindow(3);
}

/*pvoid PCloudApp::showSettings(){
    hideAllWindows();
    // if (!settingswin)
    // settingswin=new SettingsWindow(this);
    //showWindow(settingswin);
    pCloudWin->showpcloudWindow(4);

}*/

void PCloudApp::showpcloudHelp()
{
    pCloudWin->showpcloudWindow(5);
}

void PCloudApp::showpCloudAbout(){
    hideAllWindows();
    pCloudWin->showpcloudWindow(6);
}

/*p
void PCloudApp::openCloudDir(){
    QString path = settings->get("path");

#ifdef Q_OS_WIN
    int retray = 5;
    char drive = path.toUtf8().at(0);
    if (drive >= 'A' && drive <= 'Z')
        drive -= 'A';
    else if (drive >= 'a' && drive <= 'z')
        drive -= 'a';
    else return;

    while (retray-- && !isConnected(drive)){
        Sleep(1000);
    }

    if (!QProcess::startDetached("explorer.exe", QStringList(path))){
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

#else
    if (isMounted()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
#endif
}
*/
/*p void PCloudApp::shareFolder(){ // for Shares page btn
    //  hideAllWindows();
    if (!sharefolderwin)
        sharefolderwin=new ShareFolderWindow(this);
    showWindow(sharefolderwin);
}*/

void PCloudApp::logOut(){
    loggedin=false;
    /*p #ifdef Q_OS_WIN
    if (notifythread){
        notifythread->terminate();
        notifythread->wait();
        delete notifythread;
        notifythread = NULL;
    }
#endif
*/
    username="";
    psync_logout(); //sets auth to ""
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    pCloudWin->setOnlineItems(false);
    emit changeSyncIcon(OFFLINE_ICON);
    pCloudWin->hide();
    this->authentication = "";
    this->setFirstLaunch(false);
    //p unmount
}


void PCloudApp::doExit(){
    //p unMount();
    //psync_destroy();
    quit();
}

void PCloudApp::showOnClick(){
    /*p  if (loggedin)
        openCloudDir();
    else
        showLogin();
        */
    if(!loggedin)
        showLogin();
    else
        //showSync();
        showAccount();
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason){
    if (reason == QSystemTrayIcon::Trigger)
    {
        showOnClick();
        return;
    }
}

void PCloudApp::createMenus(){
    notloggedmenu=new QMenu();

    registerAction=new QAction(QIcon(":/menu/images/menu 48x48/register.png"),trUtf8 ("&Register"), this);
    connect(registerAction, SIGNAL(triggered()), this, SLOT(showRegister()));
    loginAction=new QAction(QIcon(":/menu/images/menu 48x48/login.png"),trUtf8("&Login"), this);
    connect(loginAction, SIGNAL(triggered()), this, SLOT(showLogin()));
    //p settingsAction=new QAction(trUtf8("Se&ttings"), this);
    //p connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    helpAction = new QAction(QIcon(":/menu/images/menu 48x48/help.png"),trUtf8("&Help"),this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showpcloudHelp()));
    aboutPCloudAction = new QAction(QIcon(":/menu/images/menu 48x48/info.png"),trUtf8("&About"), this);
    connect(aboutPCloudAction, SIGNAL(triggered()), this, SLOT(showpCloudAbout()));
    exitAction=new QAction(QIcon(":/menu/images/menu 48x48/exit.png"),trUtf8("&Exit"), this); // to be hidden in account tab
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));

    notloggedmenu->addAction(registerAction);
    notloggedmenu->addAction(loginAction);
    //p notloggedmenu->addAction(settingsAction);
    notloggedmenu->addAction(helpAction);
    notloggedmenu->addAction(aboutPCloudAction);
    notloggedmenu->addSeparator();
    notloggedmenu->addAction(exitAction); // to be hidden in account tab or settings


    accountAction = new QAction(QIcon(":/menu/images/menu 48x48/user.png"),trUtf8("&Account"), this); // Account tab
    connect(accountAction, SIGNAL(triggered()),this, SLOT(showAccount()));
    //p openAction=new QAction("&Open pCloud folder", this);
    //p connect(openAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));
    //p sharesAction = new QAction(trUtf8("Sha&res"),this);
    //p connect(sharesAction, SIGNAL(triggered()), this, SLOT(showShares()));
    logoutAction=new QAction("Logout", this); // to del
    connect(logoutAction, SIGNAL(triggered()), this, SLOT(logOut()));

    //sync menu
    syncAction = new QAction(QIcon(":/menu/images/menu 48x48/sync.png"),trUtf8("&Sync"),this);
    connect(syncAction, SIGNAL(triggered()), this, SLOT(showSync()));
    pauseSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/pause.png"),trUtf8("&Pause Sync"),this);
    connect(pauseSyncAction, SIGNAL(triggered()),this,SLOT(pauseSync()));
    addSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/newsync.png"),trUtf8("&Add New Sync"),this);
    connect(addSyncAction, SIGNAL(triggered()),this, SLOT(addNewSync()));
    resumeSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/resume.png"),trUtf8("Sta&rt Sync"), this);
    connect(resumeSyncAction, SIGNAL(triggered()), this, SLOT(resumeSync()));

    loggedmenu = new QMenu();
    //p loggedmenu->addAction(openAction);
    loggedmenu->addAction(accountAction);
    loggedmenu->addAction(syncAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(addSyncAction);
    //p loggedmenu->addAction(sharesAction);
    //p loggedmenu->addAction(syncAction);
    syncMenu = loggedmenu->addMenu(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),trUtf8("Sync &Folders"));
    loggedmenu->addAction(pauseSyncAction);
    loggedmenu->addAction(resumeSyncAction);
    pstatus_t status;
    psync_get_status(&status);
    if (status.status != PSTATUS_PAUSED)
    {
        resumeSyncAction->setVisible(false);
        pCloudWin->ui->btnResumeSync->setVisible(false);
    }
    else
    {
        pauseSyncAction->setVisible(false);
        pCloudWin->ui->btnPauseSync->setVisible(false);
    }
    syncDownldAction = new QAction(QIcon(":/menu/images/menu 48x48/download.png"),trUtf8("Everything downloaded"),this);
    syncUpldAction = new QAction(QIcon(":/menu/images/menu 48x48/upload.png"),trUtf8("Everything uploaded"),this);
    loggedmenu->addAction(syncDownldAction);
    loggedmenu->addAction(syncUpldAction);
    //p loggedmenu->addAction(settingsAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(helpAction);
    loggedmenu->addAction(aboutPCloudAction);
    loggedmenu->addSeparator();
    //loggedmenu->addAction(logoutAction); // to hide in acc tab
    loggedmenu->addAction(exitAction);

    //syncMenu->addAction(addSyncAction);
    this->createSyncFolderActions(syncMenu);


    connect(loggedmenu, SIGNAL(aboutToShow()), this, SLOT(updateSyncStatus()));
    syncDownldAction->setEnabled(false);
    syncUpldAction->setEnabled(false);

}

void status_callback(pstatus_t *status)
{    
    mutex.lock();
    quint32 err = psync_get_last_error();
    if(err)
        qDebug()<<"last error: "<<err;
    switch(status->status)
    {
    case PSTATUS_READY:                     //0
        qDebug()<<"PSTATUS_READY";
        if (PCloudApp::appStatic->isLogedIn()) //
        {
            PCloudApp::appStatic->changeSyncIconPublic(SYNCED_ICON);

            if(PCloudApp::appStatic->downldFlag)
            {
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything downloaded");
                PCloudApp::appStatic->downldFlag = 0;
            }
            if( PCloudApp::appStatic->upldFlag)
            {
                PCloudApp::appStatic->uplodInfo = QObject::tr("Everything uploaded");
                PCloudApp::appStatic->upldFlag = 0;
            }
            //qDebug()<<"upld flag "<< upldFlag << downldFlag << PCloudApp::appStatic->downldInfo << PCloudApp::appStatic->uplodInfo;
            if (PCloudApp::appStatic->isMenuorWinActive())
            {
                PCloudApp::appStatic->updateSyncStatusPublic();
            }
        }
        break;

    case PSTATUS_DOWNLOADING:               //1
        qDebug()<<"PSTATUS_DOWNLOADING";
        PCloudApp::appStatic->downldFlag = 1;
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        qDebug()<<"DOWNLOAD bytes downlaoded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed
               <<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload;
        if (PCloudApp::appStatic->isMenuorWinActive())
        {
            if (status->bytestodownload)
            {
                if(status->downloadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ") + QString::number(status->downloadspeed/1000) + "kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestodownload/status->downloadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + " files";
                }
                else
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ")  + PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) +
                            ", " +    QString::number(status->filestodownload) + " files";
            }
            else
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything downloaded");

            if(PCloudApp::appStatic->upldFlag)
            {
                PCloudApp::appStatic->uplodInfo = QObject::tr("Everything uploaded");
                PCloudApp::appStatic->upldFlag = 0;
            }

            PCloudApp::appStatic->updateSyncStatusPublic();
        }
        break;

    case PSTATUS_UPLOADING:                 //2
        qDebug()<<"PSTATUS_UPLOADING";
        PCloudApp::appStatic->upldFlag= 1;
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed
               <<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload;

        if (PCloudApp::appStatic->isMenuorWinActive())
        {
            if (status->bytestoupload)
            {
                if(status->uploadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Upload: ") + QString::number(status->uploadspeed/1000) + "kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestoupload/status->uploadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + " files";
                }
                else
                    PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Upload: ")  + PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + " files";
            }
            else
                PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Everything uploaded");

            //case when come from PSTATUS_DOWNLOADINGANDUPLOADING
            if(PCloudApp::appStatic->downldFlag)
            {
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything downloaded");
                PCloudApp::appStatic->downldFlag = 0;
            }
            PCloudApp::appStatic->updateSyncStatusPublic();
        }
        break;

    case PSTATUS_DOWNLOADINGANDUPLOADING:   //3
        qDebug()<<"PSTATUS_DOWNLOADINGANDUPLOADING";
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        PCloudApp::appStatic->upldFlag = 1; PCloudApp::appStatic->downldFlag = 1;
        if (PCloudApp::appStatic->isMenuorWinActive())
        {
            qDebug()<<"DOWNLOAD bytes downlaoded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed
                   <<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload;
            if(status->bytestodownload)
            {
                if(status->downloadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ") + QString::number(status->downloadspeed/1000) + "kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestodownload/status->downloadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + " files";
                }
                else
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ")  + PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + " files";
            }
            else
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything downloaded");


            qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed
                   <<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload;
            if(status->bytestoupload)
            {
                if(status->uploadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Upload: ") + QString::number(status->uploadspeed/1000) + "kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestoupload/status->uploadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + " files";
                }
                else
                    PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Upload: ")  + PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) +", " +
                            QString::number(status->filestoupload) + " files";
            }
            else
                PCloudApp::appStatic->uplodInfo = QObject::trUtf8("Everything uploaded");
            PCloudApp::appStatic->updateSyncStatusPublic();
        }
        break;

    case PSTATUS_LOGIN_REQUIRED:            //4
        qDebug()<<"PSTATUS_LOGIN_REQUIRED";
        PCloudApp::appStatic->changeSyncIconPublic(OFFLINE_ICON);
        break;

    case PSTATUS_BAD_LOGIN_DATA:            //5
        qDebug()<<"PSTATUS_BAD_LOGIN_DATA";
        PCloudApp::appStatic->changeSyncIconPublic(OFFLINE_ICON);
        break;

    case PSTATUS_ACCOUNT_FULL:              //6
        qDebug()<<"PSTATUS_ACCOUNT_FULL";
        PCloudApp::appStatic->changeSyncIconPublic(SYNC_FULL_ICON);
        break;

    case PSTATUS_DISK_FULL:                 //7
        qDebug()<<"PSTATUS_DISK_FULL";
        PCloudApp::appStatic->changeSyncIconPublic(SYNC_FULL_ICON);
        break;

    case PSTATUS_PAUSED:                    //8
        qDebug()<<"PSTATUS_PAUSED";
        if (PCloudApp::appStatic->isLogedIn())
            PCloudApp::appStatic->changeSyncIconPublic(PAUSED_ICON);
        //update menu -> start sync for initial login
        break;

    case PSTATUS_STOPPED:                   //9
        qDebug()<<"PSTATUS_STOPPED";
        PCloudApp::appStatic->changeSyncIconPublic(OFFLINE_ICON);
        break;

    case PSTATUS_OFFLINE:                   //10
        qDebug()<<"PSTATUS_OFFLINE";
        PCloudApp::appStatic->changeSyncIconPublic(OFFLINE_ICON);
        PCloudApp::appStatic->changeOnlineItemsPublic(false);
        break;

    case PSTATUS_CONNECTING:                //11
        qDebug()<<"PSTATUS_CONNECTING";
        break;

    case PSTATUS_SCANNING:                  //12
        qDebug()<<" PSTATUS_SCANNING";
        break;

    case PSTATUS_USER_MISMATCH:             //13
        //case when set wrong user
        qDebug()<<"PSTATUS_USER_MISMATCH";
        break;

    default:
        break;
    }
    mutex.unlock();
}
static void event_callback(psync_eventtype_t event, psync_eventdata_t data)
{    
    mutex.lock();
    qDebug()<<"Event callback" << event;
    switch(event)
    {
    case PEVENT_LOCAL_FOLDER_CREATED:
        qDebug() << "PEVENT_LOCAL_FOLDER_CREATED";
        break;
    case PEVENT_REMOTE_FOLDER_CREATED:
        qDebug()<<"PEVENT_REMOTE_FOLDER_CREATED";
        break;
    case PEVENT_FILE_DOWNLOAD_STARTED:
        qDebug()<<"PEVENT_FILE_DOWNLOAD_STARTED";
        break;
    case PEVENT_FILE_DOWNLOAD_FINISHED:
        qDebug()<<"PEVENT_FILE_DOWNLOAD_FINISHED";
        break;
    case PEVENT_FILE_DOWNLOAD_FAILED:
        qDebug()<<"PEVENT_FILE_DOWNLOAD_FAILED";
        break;
    case PEVENT_FILE_UPLOAD_STARTED:
        qDebug()<<"PEVENT_FILE_UPLOAD_STARTED";
        break;
    case PEVENT_FILE_UPLOAD_FINISHED:
        qDebug()<<"PEVENT_FILE_UPLOAD_FINISHED";
        break;
    case PEVENT_FILE_UPLOAD_FAILED:
        qDebug()<<"PEVENT_FILE_UPLOAD_FAILED";
        break;
    case PEVENT_LOCAL_FOLDER_DELETED:
        qDebug()<<"PEVENT_LOCAL_FOLDER_DELETED";
        break;
    case PEVENT_REMOTE_FOLDER_DELETED:
        qDebug()<<"PEVENT_REMOTE_FOLDER_DELETED";
        break;
    case PEVENT_LOCAL_FILE_DELETED:
        qDebug()<<"PEVENT_LOCAL_FILE_DELETED";
        break;
    case PEVENT_REMOTE_FILE_DELETED:
        qDebug()<<"PEVENT_REMOTE_FILE_DELETED";
        break;
    case PEVENT_LOCAL_FOLDER_RENAMED:
        qDebug()<<"PEVENT_LOCAL_FOLDER_RENAMED";
        break;
    case PEVENT_USERINFO_CHANGED:
        qDebug()<<"PEVENT_USERINFO_CHANGED";
        if (PCloudApp::appStatic->isLogedIn())
            PCloudApp::appStatic->updateUserInfoPublic("userinfo");
        break;
    case PEVENT_USEDQUOTA_CHANGED:
        qDebug()<<"PEVENT_USEDQUOTA_CHANGED";
        if (PCloudApp::appStatic->isLogedIn())
            PCloudApp::appStatic->updateUserInfoPublic("quota");
        break;
    case PEVENT_SHARE_REQUESTIN:
        qDebug()<<"PEVENT_SHARE_REQUESTIN";
        break;
    case PEVENT_SHARE_REQUESTOUT:
        qDebug()<<"PEVENT_SHARE_REQUESTOUT";
        break;
    case PEVENT_SHARE_ACCEPTIN:
        qDebug()<<"PEVENT_SHARE_ACCEPTIN";
        break;
    case PEVENT_SHARE_ACCEPTOUT:
        qDebug()<<"PEVENT_SHARE_ACCEPTOUT";
        break;
    case PEVENT_SHARE_DECLINEIN:
        qDebug()<<"PEVENT_SHARE_DECLINEIN";
        break;
    case PEVENT_SHARE_DECLINEOUT:
        qDebug()<<"PEVENT_SHARE_DECLINEOUT";
        break;
    case PEVENT_SHARE_CANCELIN:
        qDebug()<<"PEVENT_SHARE_CANCELIN";
        break;
    case PEVENT_SHARE_CANCELOUT:
        qDebug()<<"PEVENT_SHARE_CANCELOUT";
        break;
    case PEVENT_SHARE_REMOVEIN:
        qDebug()<<"PEVENT_SHARE_REMOVEIN";
        break;
    case PEVENT_SHARE_REMOVEOUT:
        qDebug()<<"PEVENT_SHARE_REMOVEOUT";
        break;
    case PEVENT_SHARE_MODIFYIN:
        qDebug()<<"PEVENT_SHARE_MODIFYIN";
        break;
    case PEVENT_SHARE_MODIFYOUT:
        qDebug()<<"PEVENT_SHARE_MODIFYOUT";
        break;
    default:
        break;

    }
    mutex.unlock();
}

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{

#ifdef Q_OS_WIN
    //   notifythread = NULL;
#endif
    appStatic = this;
    regwin=NULL;
    logwin=NULL;
    loggedmenu=NULL;
    //p sharefolderwin=NULL;
    welcomeWin = NULL;
    isFirstLaunch = false;
    //p mthread=NULL;
    loggedin=false;
    lastMessageType=-1;
    settings=new PSettings(this);
    upldFlag = 0;
    downldFlag = 0;
    bytestoDwnld = 0;
    bytestoUpld = 0;
    downldInfo = QObject::trUtf8("Everything downloaded");
    uplodInfo = QObject::trUtf8("Everything uploaded");
    unlinkFlag = false;
    isCursorChanged = false;
    tray=new QSystemTrayIcon(QIcon(OFFLINE_ICON),this);
#ifdef Q_OS_LINUX    
    if(QT_VERSION < QT_VERSION_CHECK(5,0,0))
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")); // for non-latin strings
#endif
    tray->setToolTip("pCloud");
    tray->show();
    if (psync_init() == -1)
    {
        QMessageBox::critical(NULL, "pCloud sync", tr("pCloud sync has stopped. Please connect to our support"));
        qDebug()<<" psync-init returned -1 "<<psync_get_last_error();
        this->quit();
    }
    psync_start_sync(status_callback,event_callback);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(;;)
    {
        pstatus_t status;
        psync_get_status(&status);
        if (status.status == PSTATUS_CONNECTING || status.status == PSTATUS_SCANNING )
        {
            sleep(1);
            continue;
        }
        else
            break;
    }
    QApplication::restoreOverrideCursor();
    pCloudWin = new PCloudWindow(this);  //needs settings to be created
    pCloudWin->layout()->setSizeConstraint(QLayout::SetFixedSize); //for auto resize
    pCloudWin->setOnlineItems(false);
    createMenus(); //needs sync to be started
    tray->setContextMenu(notloggedmenu);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    //p connect(tray, SIGNAL(messageClicked()), this, SLOT(trayMsgClicked()));
    connect(this, SIGNAL(changeSyncIcon(QString)), this, SLOT(setTrayIcon(QString)));
    connect(this, SIGNAL(changeOnlineItemsSgnl(bool)), this, SLOT(changeOnlineItems(bool)));
    connect(this, SIGNAL(changeCursor(bool)), this, SLOT(setCursor(bool)));
    connect(this, SIGNAL(sendErrText(int, const char*)), this, SLOT(setErrText(int,const char*)));
    connect(this, SIGNAL(updateSyncStatusSgnl()), this, SLOT(updateSyncStatus()));
    connect(this,SIGNAL(updateUserInfoSgnl(const char* &)), this, SLOT(updateUserInfo(const char* &)));

    bool savedauth = psync_get_bool_value("saveauth"); //works when syns is paused also
    if (!savedauth)
    {
        //case not remembered
        //p othread=NULL;
        QString name = psync_get_username();
        if (name == "") // case after unlink
            this->isFirstLaunch = true;
        else
            this->isFirstLaunch = false;
        showLogin();
    }
    else
        logIn(psync_get_username(),true);

    cfg = manager.defaultConfiguration();
    session = new QNetworkSession(cfg);
    session->open();
    connect(session, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(networkConnectionChanged(QNetworkSession::State)));
    //for case when upld is called only once
    pstatus_t status;
    status_callback(&status);
    /* p
        else
        othread=new OnlineThread(this);
        othread->start();
    }p */


}

PCloudApp::~PCloudApp(){
    /*p if (othread){
        if (othread->isRunning())
            othread->terminate();
        othread->wait();
        delete othread;
    }
    if (mthread){
        if (mthread->isRunning())
            mthread->terminate();
        mthread->wait();
        delete mthread;
    } p*/
    psync_destroy();
    delete settings;
    delete tray;
    if (loggedmenu)
        delete loggedmenu;
    delete notloggedmenu;
    delete registerAction;
    delete loginAction;
    delete exitAction;
    delete logoutAction;
    //p delete openAction;
    //p delete settingsAction;
    //p delete sharesAction;
    delete syncAction;
    delete helpAction;
    delete aboutPCloudAction;
    if (regwin)
        delete regwin;
    if (logwin)
        delete logwin;
    /*p if (settingswin)
        delete settingswin;
    if (sharefolderwin)
        delete sharefolderwin;   p*/
    if (pCloudWin)
        delete pCloudWin;
    if (welcomeWin)
        delete welcomeWin;
}

void PCloudApp::check_error()
{
    quint32 err = psync_get_last_error();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    switch (err)
    {
    case PERROR_LOCAL_FOLDER_NOT_FOUND: //1
        qDebug()<< PERROR_LOCAL_FOLDER_NOT_FOUND;
        break;
    case PERROR_REMOTE_FOLDER_NOT_FOUND: //2
        qDebug()<<PERROR_REMOTE_FOLDER_NOT_FOUND;
        break;
    case PERROR_DATABASE_OPEN: //3
        msgBox.setText(trUtf8("Database open error!"));
        msgBox.setInformativeText(trUtf8("Please Check your free disk space or contact our support."));
        msgBox.exec();
        // ++ exit
        break;
    case PERROR_NO_HOMEDIR: //4
        msgBox.setText(trUtf8("No home directory!"));
        msgBox.setInformativeText(trUtf8("Please contact our support"));
        msgBox.exec();
        // ++ exit
        break;
    case PERROR_SSL_INIT_FAILED: //5
        msgBox.setText(trUtf8("SSL initialization failed!"));
        msgBox.setInformativeText(trUtf8("Please contact our support"));
        msgBox.exec();
        // ++ exit
        break;
    case PERROR_DATABASE_ERROR: //6
        msgBox.setText(trUtf8("Database error!"));
        msgBox.setInformativeText(trUtf8("Please check your free disk space or contact support."));
        msgBox.exec();
        // ++ exit
        break;
    case PERROR_LOCAL_FOLDER_ACC_DENIED: //7
        msgBox.setText(trUtf8("Can not add new sync: Local folder access denied!"));
        msgBox.setInformativeText(trUtf8("Please check folder permissions according to sync type"));
        msgBox.exec();
        break;
    case PERROR_REMOTE_FOLDER_ACC_DENIED: //8
        msgBox.setText(trUtf8("Can not add new sync: Remote folder access denied!"));
        msgBox.setInformativeText(trUtf8("Please check folder permissions according to sync type"));
        msgBox.exec();
        break;
    case PERROR_FOLDER_ALREADY_SYNCING: //9
        msgBox.setText(trUtf8("Can not add new sync: Folder has already synchronized"));
        msgBox.exec();
        break;
    case PERROR_INVALID_SYNCTYPE:  //10
        qDebug()<< PERROR_INVALID_SYNCTYPE;
        break;
    case PERROR_OFFLINE: //11
        msgBox.setText(trUtf8("Internal error!"));
        msgBox.setInformativeText(trUtf8("pCloud is offline now. Please reconnect"));
        msgBox.exec();
        break;
    case PERROR_INVALID_SYNCID: //12
        qDebug()<<PERROR_INVALID_SYNCID;
        break;
    case PERROR_PARENT_OR_SUBFOLDER_ALREADY_SYNCING: //13
        msgBox.setText(trUtf8("Can not add new sync: Parent folder or subfolder of it has already synchronized!"));
        msgBox.setInformativeText(trUtf8("Please check your synchronized folders list"));
        msgBox.exec();
        break;
    default:
        break;
    }
}

void PCloudApp::showError(QString &err){
    tray->showMessage("Error", err, QSystemTrayIcon::Warning);
}
/*
void PCloudApp::showTrayMessage(QString title, QString msg)
{
    tray->showMessage(title, msg, QSystemTrayIcon::Information);
}*/

void PCloudApp::logIn(const QString &uname, bool remember) //needs STATUS_READY
{
    if (this->unlinkFlag)
    {
        syncMenu->clear();
        resumeSyncAction->setVisible(false);
        pauseSyncAction->setVisible(true);
        this->downldInfo = QObject::trUtf8("Everything downloaded");
        this->uplodInfo = QObject::trUtf8("Everything uploaded");
        this->pCloudWin->get_sync_page()->load();
        this->pCloudWin->get_sync_page()->loadSettings();
        pCloudWin->ui->btnResumeSync->setVisible(false);
        pCloudWin->ui->btnPauseSync->setVisible(true);
        this->unlinkFlag = false;
    }
    this->username = uname;
    this->rememberMe = remember;
    this->getUserInfo();
    this->loggedin=true;
    //p  setSettings(); // for pcloud
    tray->setToolTip(username);
    //if (loggedmenu){
    //loggedmenu->actions()[0]->setText(username);
    //}
    pCloudWin->setOnlineItems(true);
    pCloudWin->setOnlinePages();
    pstatus_t status;
    psync_get_status(&status);
    if (status.status != PSTATUS_PAUSED)
        tray->setIcon(QIcon(SYNCED_ICON));
    else
        tray->setIcon(QIcon(PAUSED_ICON));
    tray->setContextMenu(loggedmenu);
    // isFirstLaunch = true; // for test
    if (isFirstLaunch)
    {
        welcomeWin = new WelcomeScreen(this);
        this->showWindow(welcomeWin);
    }
    else
        showAccount();

}
void PCloudApp::getUserInfo()
{
    this->authentication = psync_get_auth_string();
    this->isVerified = psync_get_bool_value("emailverified");
    this->isPremium = psync_get_bool_value("premium");
    this->getQuota();
}
void PCloudApp::getQuota()
{
    quint64 quota = psync_get_uint_value("quota");
    if (quota){
        this->planStr =  QString::number(quota >> 30 ) + " GB";
        quint64 usedquota =  psync_get_uint_value("usedquota");
        if (!usedquota)
        {
            while (!usedquota)
            {
                usedquota =  psync_get_uint_value("usedquota");
                sleep(1);
            }
        }
        qDebug() << quota<< "used quota " << usedquota;
        this->usedSpace = static_cast<double>(usedquota) / (1<<30);
        this->freeSpacePercentage = (100*(quota - usedquota))/quota;
    }
}

/*p
void PCloudApp::trayMsgClicked()
{
    if (lastMessageType == 0 || lastMessageType == 1 )
    {
        emit showShares();
        pCloudWin->ui->tabWidgetShares->setCurrentIndex(lastMessageType);
        pCloudWin->sharesPage->load(lastMessageType);

    }
}

// mth oth
void PCloudApp::setOnlineStatus(bool online)
{
    static bool lastStatus = false;
    if (online){
        tray->setIcon(QIcon(ONLINE_ICON));
        if (online != lastStatus) {
            lastMessageType = 2;
            tray->showMessage("PCloud connected", "", QSystemTrayIcon::Information);
            lastStatus = online;
        }
    }
    else{
        tray->setIcon(QIcon(OFFLINE_ICON));
        if (online != lastStatus){
            lastMessageType = 2;
            tray->showMessage("PCloud disconnected", "", QSystemTrayIcon::Information);
            lastStatus = online;
        }
    }
}
p */

bool PCloudApp::isLogedIn()
{
    return loggedin;
}

bool PCloudApp::isMenuorWinActive()
{
    if(this->loggedmenu)
        return (this->loggedmenu->isActiveWindow() || this->pCloudWin->isVisible());
    //return this->loggedmenu->isVisible();
    else
        return false;
}

// use public function to change sync icon according to statuses
//because static vars can't emit signals( signals are protected i qt4)
void PCloudApp::changeSyncIconPublic(const QString &icon)
{
    emit this->changeSyncIcon(icon);
}
void PCloudApp::changeOnlineItemsPublic(bool logged)
{
    emit this->changeOnlineItemsSgnl(logged);
}

void PCloudApp::changeCursorPublic(bool change)
{
    emit this->changeCursor(change);
}
void PCloudApp::setTextErrPublic(int win, const char *err)
{
    emit this->sendErrText(win,err);
}
void PCloudApp::updateSyncStatusPublic()
{
    emit this->updateSyncStatusSgnl();

}
void PCloudApp::updateUserInfoPublic(const char* param)
{
    emit updateUserInfoSgnl(param);
    emit this->pCloudWin->refreshUserinfo();
}

void PCloudApp::setErrText(int win, const char *err)
{
    switch(win)
    {
    case 1:
        if (this->logwin)
            logwin->setError(err);
        break;
    default:
        break;
    }
}

void PCloudApp::setLogWinError(const char *msg)
{
    if (this->logwin)
        this->logwin->setError(msg);
}
void PCloudApp::setTrayIcon(const QString &icon)
{
    tray->setIcon(QIcon(icon));
}
void PCloudApp::setCursor(bool change)
{
    if (change)
    {
        this->isCursorChanged = true; // flag for status_callback
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    else
    {
        this->isCursorChanged = false;
        //QApplication::restoreOverrideCursor(); // work only in debug
        QApplication::changeOverrideCursor(Qt::ArrowCursor);
    }
}

void PCloudApp::pauseSync()
{
    psync_pause();
    pauseSyncAction->setVisible(false);
    resumeSyncAction->setVisible(true);
    pCloudWin->ui->btnPauseSync->setVisible(false);
    pCloudWin->ui->btnResumeSync->setVisible(true);
}
void PCloudApp::resumeSync()
{
    psync_resume();
    pauseSyncAction->setVisible(true);
    resumeSyncAction->setVisible(false);
    pCloudWin->ui->btnPauseSync->setVisible(true);
    pCloudWin->ui->btnResumeSync->setVisible(false);
}

void PCloudApp::createSyncFolderActions(QMenu *syncMenu)
{
    syncMenu->clear();
    psync_folder_list_t *fldrsList = psync_get_sync_list();
    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (uint i = 0; i< fldrsList->foldercnt; i++)
        {
            QAction *fldrAction = new QAction(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),fldrsList->folders[i].localname,this);
            fldrAction->setProperty("path", fldrsList->folders[i].localpath);
            connect(fldrAction, SIGNAL(triggered()),this, SLOT(openLocalDir()));
            syncMenu->addAction(fldrAction);
        }
    }
    free(fldrsList);
}
QMenu* PCloudApp::getSyncMenu()
{
    return this->syncMenu;
}

//when user selects it from the menu
void PCloudApp::openLocalDir()
{
    QObject *menuItem = QObject::sender();
    QString path = menuItem->property("path").toString();
    qDebug()<<"open local folder from the menu "<<path;
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
void PCloudApp::addNewFolderInMenu(QAction *fldrAction)
{
    this->syncMenu->addAction(fldrAction);
}
void PCloudApp::addNewSync()
{
    emit this->pCloudWin->syncPage->addSync(); // to be moved
}
//updates menu, pcloudwin and tray icon with current sync upld/downld info
void PCloudApp::updateSyncStatus()
{
    QString traymsg = this->username + "\n" + this->downldInfo + "\n" + this->uplodInfo;
    syncDownldAction->setText(downldInfo);
    syncUpldAction->setText(uplodInfo);
    pCloudWin->ui->label_dwnld->setText(downldInfo);
    pCloudWin->ui->label_upld->setText(uplodInfo);

    this->tray->setToolTip(traymsg);
}
void PCloudApp::updateUserInfo(const char* &param)
{    
    if (param == "quota")
        this->getQuota();
    else
        this->getUserInfo();
}
void PCloudApp::changeOnlineItems(bool logged)
{
    if(logged)
    {
        tray->setContextMenu(loggedmenu);
        pCloudWin->setOnlineItems(true);
    }
    else
    {
        tray->setContextMenu(notloggedmenu);
        if (pCloudWin)
        {
            pCloudWin->setOnlineItems(false);
            pCloudWin->hide();
        }
    }
}

void PCloudApp::setFirstLaunch(bool b)
{
    this->isFirstLaunch = b;
}

QString PCloudApp::bytesConvert(quint64 bytes)
{
    if(bytes < 1<<10)
        return QString::number(bytes) + " B";

    if(bytes < 1<<20)
        return QString::number(bytes>>10) + " KB";

    if(bytes < 1<<30)
        return QString::number(bytes/(1<<20)) + " MB";

    quint64 one = 1;
    if(bytes< one<<40)
    {
        qreal res = static_cast<double>(bytes) / (1<<30);
        return QString::number(res, 'f', 1) + " GB";
    }
    else
    {
        qreal res = static_cast<double>(bytes) / (one<<40);
        return QString::number(res, 'f' ,1) + " TB";
    }
}
QString PCloudApp::timeConvert(quint64 seconds)
{
    if (seconds < 60)
        return QString::number(seconds) + "s";
    if(seconds < 3600)
        return QString::number(seconds/60) + " m";
    else
        return QString::number(seconds/3600) + "h and " + QString::number((seconds%3600)/60) + " m";
}

void PCloudApp::networkConnectionChanged(QNetworkSession::State state)
{
    qDebug()<<"network connection state changed " << state;
    if (state == QNetworkSession::NotAvailable || state == QNetworkSession::Connected)
        psync_network_exception();
}

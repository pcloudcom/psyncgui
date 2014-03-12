#include "pcloudapp.h"
#include "common.h"
#include <QMenu>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <unistd.h>
#include <QDebug> //temp
#include "unistd.h" //for sync statuses
#include <QWidgetAction> //temp maybe


PCloudApp * PCloudApp::appStatic = NULL;

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
    tray->setIcon(QIcon(OFFLINE_ICON));
    pCloudWin->setOnlineItems(false);
    pCloudWin->hide();
    this->authentication = "";
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
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason){
    if (reason==QSystemTrayIcon::Trigger)
        showOnClick();
}

void PCloudApp::createMenus(){
    notloggedmenu=new QMenu();

    registerAction=new QAction(trUtf8 ("&Register"), this);
    connect(registerAction, SIGNAL(triggered()), this, SLOT(showRegister()));
    loginAction=new QAction(trUtf8("&Login"), this);
    connect(loginAction, SIGNAL(triggered()), this, SLOT(showLogin()));
    //p settingsAction=new QAction(trUtf8("Se&ttings"), this);
    //p connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    helpAction = new QAction(trUtf8("&Help"),this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showpcloudHelp()));
    aboutPCloudAction = new QAction(trUtf8("&About"), this);
    connect(aboutPCloudAction, SIGNAL(triggered()), this, SLOT(showpCloudAbout()));
    exitAction=new QAction(trUtf8("&Exit"), this); // to be hidden in account tab
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));

    notloggedmenu->addAction(registerAction);
    notloggedmenu->addAction(loginAction);
    //p notloggedmenu->addAction(settingsAction);
    notloggedmenu->addAction(helpAction);
    notloggedmenu->addAction(aboutPCloudAction);
    notloggedmenu->addSeparator();
    notloggedmenu->addAction(exitAction); // to be hidden in account tab or settings


    accountAction = new QAction(trUtf8("&Account"), this); // Account tab
    connect(accountAction, SIGNAL(triggered()),this, SLOT(showAccount()));
    //p openAction=new QAction("&Open pCloud folder", this);
    //p connect(openAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));
    //p sharesAction = new QAction(trUtf8("Sha&res"),this);
    //p connect(sharesAction, SIGNAL(triggered()), this, SLOT(showShares()));
    logoutAction=new QAction("Logout", this);
    connect(logoutAction, SIGNAL(triggered()), this, SLOT(logOut()));

    //sync menu
    syncAction = new QAction(trUtf8("&Sync"),this);
    connect(syncAction, SIGNAL(triggered()), this, SLOT(showSync()));
    pauseSyncAction = new QAction(trUtf8("&Pause Sync"),this);
    connect(pauseSyncAction, SIGNAL(triggered()),this,SLOT(pauseSync()));
    addSyncAction = new QAction(trUtf8("&Add New Sync"),this);
    connect(addSyncAction, SIGNAL(triggered()),this, SLOT(addNewSync()));
    resumeSyncAction = new QAction(trUtf8("Sta&rt Sync"), this);
    connect(resumeSyncAction, SIGNAL(triggered()), this, SLOT(resumeSync()));

    loggedmenu = new QMenu();
    //p loggedmenu->addAction(openAction);
    loggedmenu->addAction(accountAction);
    loggedmenu->addAction(addSyncAction);
    //p loggedmenu->addAction(sharesAction);
    //p loggedmenu->addAction(syncAction);
    syncMenu = loggedmenu->addMenu(trUtf8("Sync &Folders"));
    loggedmenu->addAction(syncAction);
    loggedmenu->addAction(pauseSyncAction);
    loggedmenu->addAction(resumeSyncAction);
    pstatus_t status;
    psync_get_status(&status);
    if (status.status != PSTATUS_PAUSED)
        resumeSyncAction->setVisible(false);
    else
        pauseSyncAction->setVisible(false);
    syncDownldAction = new QAction(QIcon(":/images/images/arrowdown.png"),trUtf8("Everything downloaded"),this);
    syncUpldAction = new QAction(QIcon(":/images/images/arrowup.png"),trUtf8("Everything uploaded"),this);
    loggedmenu->addAction(syncDownldAction);
    loggedmenu->addAction(syncUpldAction);
    //p loggedmenu->addAction(settingsAction);
    loggedmenu->addAction(helpAction);
    loggedmenu->addAction(aboutPCloudAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(logoutAction); // to hide in acc tab
    loggedmenu->addAction(exitAction);

    //syncMenu->addAction(addSyncAction);
    this->createSyncFolderActions(syncMenu);


    //create upload/download info at the menu
  /*  QWidgetAction *syncWdgtAction = new QWidgetAction(loggedmenu);
    syncStatusListWidget = new QListWidget(loggedmenu);
    syncStatusListWidget->setFlow(QListWidget::LeftToRight);
    syncStatusListWidget->setMovement(QListView::Static);
    syncStatusListWidget->setMaximumHeight(100);
    new QListWidgetItem(QIcon(":/images/images/arrowdown.png"),trUtf8("Everything\ndownloaded"),syncStatusListWidget); //index 0 download
    syncStatusListWidget->item(0)->setFlags(Qt::NoItemFlags);
    syncStatusListWidget->item(0)->setForeground(*(new QBrush(Qt::black)));
    new QListWidgetItem(QIcon(":/images/images/arrowup.png"),trUtf8("Everything\nuploaded"),syncStatusListWidget); //index 1 upload
    syncStatusListWidget->item(1)->setFlags(Qt::NoItemFlags);
    syncStatusListWidget->item(1)->setForeground(*(new QBrush(Qt::black)));

    syncWdgtAction->setDefaultWidget(syncStatusListWidget);
    loggedmenu->addAction(syncWdgtAction); */
    connect(loggedmenu, SIGNAL(aboutToShow()), this, SLOT(updateSyncStatusInMenu()));

}

void status_callback(pstatus_t *status)
{
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
            //if (PCloudApp::appStatic->isCursorChanged) // after connecting, scaning
            // PCloudApp::appStatic->changeCursorPublic(false);
            if (PCloudApp::appStatic->bytestoDwnld)
            {
                PCloudApp::appStatic->bytestoDwnld = 0; // to keep it in QMap<str, val>
                PCloudApp::appStatic->filesToDwnld = 0;
                PCloudApp::appStatic->dwnldSpeed = 0;
            }
            if(PCloudApp::appStatic->bytestoUpld)
            {
                PCloudApp::appStatic->bytestoUpld = 0;
                PCloudApp::appStatic->filesToUpld = 0;
                PCloudApp::appStatic->upldSpeed = 0;
            }
            if (PCloudApp::appStatic->isMenuActive())
            {
                PCloudApp::appStatic->updateSyncStatusInMenu();
            }
        }
        break;

    case PSTATUS_DOWNLOADING:               //1
        qDebug()<<"PSTATUS_DOWNLOADING";
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        qDebug()<<"DOWNLOAD bytes downlaoded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed;
        qDebug()<<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload;
        if (PCloudApp::appStatic->isMenuActive())
        {
            PCloudApp::appStatic->bytestoDwnld = status->bytestodownload;
            PCloudApp::appStatic->filesToDwnld = status->filestodownload;
            if(status->downloadspeed)// sometimes is 0
                PCloudApp::appStatic->dwnldSpeed = status->downloadspeed;
            PCloudApp::appStatic->updateSyncStatusInMenu();
        }
        break;

    case PSTATUS_UPLOADING:                 //2
        qDebug()<<"PSTATUS_UPLOADING";
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed;
        qDebug()<<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload;
        if (PCloudApp::appStatic->isMenuActive())
        {
            PCloudApp::appStatic->bytestoUpld = status->bytestoupload;
            PCloudApp::appStatic->filesToUpld = status->filestoupload;
            if(status->uploadspeed)// sometimes is 0
                PCloudApp::appStatic->upldSpeed = status->uploadspeed;
            PCloudApp::appStatic->updateSyncStatusInMenu();
        }
        break;

    case PSTATUS_DOWNLOADINGANDUPLOADING:   //3
        qDebug()<<"PSTATUS_DOWNLOADINGANDUPLOADING";
        PCloudApp::appStatic->changeSyncIconPublic(SYNCING_ICON);
        if (PCloudApp::appStatic->isMenuActive())
        {
            qDebug()<<"DOWNLOAD bytes downlaoded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed;
            qDebug()<<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload;
            PCloudApp::appStatic->bytestoDwnld = status->bytestodownload;
            PCloudApp::appStatic->filesToDwnld = status->filestodownload;
            if(status->downloadspeed)// sometimes is 0
                PCloudApp::appStatic->dwnldSpeed = status->downloadspeed;

            qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed;
            qDebug()<<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload;
            PCloudApp::appStatic->bytestoUpld = status->bytestoupload;
            PCloudApp::appStatic->filesToUpld = status->filestoupload;
            if(status->uploadspeed)// sometimes is 0
                PCloudApp::appStatic->upldSpeed = status->uploadspeed;
            PCloudApp::appStatic->updateSyncStatusInMenu();
        }
        break;

    case PSTATUS_LOGIN_REQUIRED:            //4
        qDebug()<<"PSTATUS_LOGIN_REQUIRED";
        break;

    case PSTATUS_BAD_LOGIN_DATA:            //5
        qDebug()<<"PSTATUS_BAD_LOGIN_DATA";
        //PCloudApp::appStatic->setLogWinError("Invalid user and password combination");
        // PCloudApp::appStatic->setTextErrPublic(1,"Invalid user and password combination"); //loginwin - managed from the win
        /*if (PCloudApp::appStatic->isLogedIn())
        {
            PCloudApp::appStatic->logOut();
            emit PCloudApp::appStatic->showLogin();
        }*/
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
        break;

    case PSTATUS_CONNECTING:                //11
        qDebug()<<"PSTATUS_CONNECTING";
        // PCloudApp::appStatic->changeSyncIconPublic(":/images/images/syncing"); // or offlineicon
        //PCloudApp::appStatic->changeCursorPublic(true);
        break;

    case PSTATUS_SCANNING:                  //12
        qDebug()<<" PSTATUS_SCANNING";
        //PCloudApp::appStatic->changeSyncIconPublic(":/images/images/syncing"); - to del
        // PCloudApp::appStatic->changeCursorPublic(true);
        break;

    case PSTATUS_USER_MISMATCH:             //13
        //case when set wrong user
        // PCloudApp::appStatic->setTextErrPublic(1,"Invalid user and password combination"); //loginwin regwin
        qDebug()<<"PSTATUS_USER_MISMATCH";
        break;

    default:
        break;
    }

}
static void event_callback(psync_eventtype_t event, psync_syncid_t syncid, psync_fileorfolderid_t remoteid, const char *name, const char *localpath, const char *remotepath)
{
    qDebug()<<event << " " << syncid<< " " << name <<" " << localpath <<" " <<remotepath<< " "<<remoteid;
    switch(event)
    {
    case PEVENT_FILE_DOWNLOAD_FINISHED:
        qDebug()<<"PEVENT_FILE_DOWNLOAD_FINISHED";
        break;
    case PEVENT_FILE_UPLOAD_FINISHED:
        qDebug()<<"PEVENT_FILE_UPLOAD_FINISHED";
        break;
    case PEVENT_LOCAL_FOLDER_DELETED:
        qDebug()<<PEVENT_LOCAL_FOLDER_DELETED;
        //refresh lists
        break;
        // ++ event za changed pass
    default:
        break;

    }
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
    bytestoDwnld = 0;
    bytestoUpld = 0;
    filesToDwnld = 0;
    filesToUpld = 0;
    dwnldSpeed = 0;
    upldSpeed = 0;
    isCursorChanged = false;
    psync_init(); // toadd checks
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
    tray=new QSystemTrayIcon(this);
    tray->setIcon(QIcon(OFFLINE_ICON));
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    //p connect(tray, SIGNAL(messageClicked()), this, SLOT(trayMsgClicked()));
    connect(this, SIGNAL(changeSyncIcon(QString)), this, SLOT(setTrayIcon(QString)));
    connect(this, SIGNAL(changeCursor(bool)), this, SLOT(setCursor(bool)));
    connect(this, SIGNAL(sendErrText(int, const char*)), this, SLOT(setErrText(int,const char*)));
    tray->show();

    // to move in status_ready
    //QString auth = psync_get_auth_string();
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


void PCloudApp::showError(QString err){
    tray->showMessage("Error", err, QSystemTrayIcon::Warning);
}
/*
void PCloudApp::showTrayMessage(QString title, QString msg)
{
    tray->showMessage(title, msg, QSystemTrayIcon::Information);
}*/

void PCloudApp::logIn(QString uname, bool remember) //needs STATUS_READY
{
    this->username = uname;
    this->rememberMe = remember;
    this->authentication = psync_get_auth_string();
    this->loggedin=true;
    this->isVerified = psync_get_bool_value("emailverified");
    this->isPremium = psync_get_bool_value("premium");
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
    //isFirstLaunch = true; // for test
    if (isFirstLaunch)
    {
        welcomeWin = new WelcomeScreen(this);
        this->showWindow(welcomeWin);
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

bool PCloudApp::isMenuActive()
{
    if(this->loggedmenu)
        return this->loggedmenu->isActiveWindow();
    //return this->loggedmenu->isVisible();
    else
        return false;
}

// use public function to change sync icon according to statuses
//because static vars can't emit signals( signals are protected i qt4)
void PCloudApp::changeSyncIconPublic(const QString icon)
{
    emit this->changeSyncIcon(icon);
}

void PCloudApp::changeCursorPublic(bool change)
{
    emit this->changeCursor(change);
}
void PCloudApp::setTextErrPublic(int win, const char *err)
{
    emit this->sendErrText(win,err);
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
void PCloudApp::setTrayIcon(const QString icon)
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
}
void PCloudApp::resumeSync()
{
    psync_resume();
    pauseSyncAction->setVisible(true);
    resumeSyncAction->setVisible(false);
}

void PCloudApp::createSyncFolderActions(QMenu *syncMenu)
{
    syncMenu->clear();
    psync_folder_list_t *fldrsList = psync_get_sync_list();
    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (int i = 0; i< fldrsList->foldercnt; i++)
        {
            QAction *fldrAction = new QAction(fldrsList->folders[i].localname,this);
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
void PCloudApp::updateSyncStatusInMenu()
{
    qDebug()<<"update menu DOWNLOAD " << bytestoDwnld << bytesConvert(bytestoDwnld) << filesToDwnld << dwnldSpeed;// << timeConvert(bytestoDwnld/dwnldSpeed);
    if(bytestoDwnld)
        //{ syncStatusListWidget->item(0)->setText(bytesConvert(bytestoDwnld) + " left\n" + QString::number(filesToDwnld) + " files\n"
        //+ timeConvert(bytestoDwnld/dwnldSpeed));
    {
        if (dwnldSpeed)
        {
           // syncDownldAction->setText(trUtf8("Remaining: ") + bytesConvert(bytestoDwnld) + " " + QString::number(filesToDwnld) + " files "
             //                         + timeConvert(bytestoDwnld/dwnldSpeed));

            syncDownldAction->setText(timeConvert(bytestoDwnld/dwnldSpeed) + trUtf8("remaining: ") + bytesConvert(bytestoDwnld) + " " + QString::number(filesToDwnld) + " files ");
            syncDownldAction->setToolTip(trUtf8("Download speed: ") + QString::number(dwnldSpeed/1000) + "kB/s");

        //    syncStatusListWidget->item(0)->setText(trUtf8("Remaining:\n") + bytesConvert(bytestoDwnld) + "\n" + QString::number(filesToDwnld) + " files\n"
            //                                       + timeConvert(bytestoDwnld/dwnldSpeed));
          //  syncStatusListWidget->item(0)->setToolTip(trUtf8("Download speed: ") + QString::number(dwnldSpeed/1000) + "kB/s");
            qDebug()<<"dwnld speed: "<< timeConvert(bytestoDwnld/dwnldSpeed);
        }
        else
            syncDownldAction->setText(trUtf8("Remaining: ") + bytesConvert(bytestoDwnld) + " " + QString::number(filesToDwnld) + " files ");
            //syncStatusListWidget->item(0)->setText(bytesConvert(bytestoDwnld) + " left\n" + QString::number(filesToDwnld) + " files\n");

    }
    else
        //syncStatusListWidget->item(0)->setText(trUtf8("Everything\ndownloaded"));
        syncDownldAction->setText(trUtf8("Everything downloaded"));


    qDebug()<<"update menu UPLOAD " << bytestoUpld << bytesConvert(bytestoUpld) << filesToUpld<< upldSpeed;// << timeConvert(bytestoDwnld/dwnldSpeed);
    if(bytestoUpld)
    {
        if (upldSpeed)
        {
           // syncStatusListWidget->item(1)->setText(trUtf8("Remaining:\n") + bytesConvert(bytestoUpld) + "\n" + QString::number(filesToUpld) + " files\n"
            //                                       + timeConvert(bytestoUpld/upldSpeed));
            //syncStatusListWidget->item(1)->setToolTip(trUtf8("Upload speed: ") + QString::number(upldSpeed/1000) + "kB/s");
            syncUpldAction->setText(timeConvert(bytestoUpld/upldSpeed) + trUtf8("remaining: ") + bytesConvert(bytestoUpld) + " " + QString::number(filesToUpld) + " files ");
            qDebug()<<"Upld speed:" << timeConvert(bytestoUpld/upldSpeed);
        }
        else
            syncUpldAction->setText(trUtf8("Remaining: ") + bytesConvert(bytestoUpld) + " " + QString::number(filesToUpld) + " files ");
          //  syncStatusListWidget->item(1)->setText(bytesConvert(bytestoUpld) + " left " + QString::number(filesToUpld) + " files");
    }
    else
        syncUpldAction->setText(trUtf8("Everything uploaded"));
        //syncStatusListWidget->item(1)->setText(trUtf8("Everything\nuploaded"));

}
void PCloudApp::setFirstLaunch(bool b)
{
    this->isFirstLaunch = b;
}

QString PCloudApp::bytesConvert(quint64 bytes)
{
    if(bytes >= 0 && bytes < 1<<10)
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
    QString mins;
    (seconds % 60) ? mins = " mins" : mins = " min";
    if(seconds < 3600)
        return QString::number(seconds/60) + mins;
    else
        return QString::number(seconds/3600) + "h and " + QString::number((seconds%3600)/60) + mins;
}

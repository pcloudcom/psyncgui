#include "pcloudapp.h"
#include "common.h"
#include <QDesktopWidget>
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
#include <QProcess>
#include "ui_pcloudwindow.h"

#ifdef Q_OS_WIN
#include <windows.h> //TEMP
#endif

PCloudApp* PCloudApp::appStatic = NULL;
QMutex mutex(QMutex::Recursive);

void PCloudApp::hideAllWindows(){
    if (regwin && regwin->isVisible())
        regwin->hide();
    if (logwin && logwin->isVisible())
        logwin->close();
    if (pCloudWin && pCloudWin->isVisible())
        pCloudWin->hide();
    if(welcomeWin && welcomeWin->isVisible())
        welcomeWin->hide();
    if(introwin && introwin->isVisible())
        introwin->close();
    if(!loggedin)
    {
        if(sharefolderwin && sharefolderwin->isVisible())
            sharefolderwin->hide();
        if(syncFldrsWin && syncFldrsWin->isVisible())
            syncFldrsWin->hide();
    }
}

void PCloudApp::showWindow(QMainWindow *win)
{
    QDesktopWidget *desktop = QApplication::desktop();
    int x = (desktop->width() - win->width()) / 2;
    int y = (desktop->height() - win->height()) / 2;
    win->move(x,y); //center the win

    win->raise();
    win->activateWindow();
    win->showNormal();
    win->setWindowState(Qt::WindowActive);
    this->setActiveWindow(win);
}

void PCloudApp::showRegister()
{
    hideAllWindows();

    int currPage;
    QString user = psync_get_username();
    if (user != "") //case after logout when the user is still linked
        currPage = 1; // we have a linked user
    else
        currPage = 0; //register

    if (!regwin)
        regwin=new RegisterWindow(this,currPage);
    else
        regwin->setCurrPage(currPage);

    showWindow(regwin);


    /*

        QMessageBox msgBox;
        msgBox.setWindowTitle("pCloud Drive");
        msgBox.setText(trUtf8 ("User %1 has already linked in.").arg(user));
        msgBox.setInformativeText(trUtf8 ("Do you want to unlink %1 and continue?").arg(user));
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.addButton(trUtf8("Unlink"), QMessageBox::YesRole);
        if(msgBox.exec() != QMessageBox::Cancel)
        {
            this->unlink();
            hideAllWindows();
        }
        else
            this->showLogin();
    }
    else
    {

        if (!regwin)
            regwin=new RegisterWindow(this, 0); // register
        showWindow(regwin);
    }
    */
}
void PCloudApp::showLogin(){
    hideAllWindows();
    if (!logwin)
        logwin=new LoginWindow(this);
    showWindow(logwin);
}

void PCloudApp::showAccount()
{
    if (welcomeWin && welcomeWin->isVisible())
        return;

    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(ACCNT_LOGGED_PAGE_NUM);
    this->showWindow(pCloudWin);
}

void PCloudApp::showDrive() // osbolete specif
{
    if(psync_fs_isstarted())
        emit this->openCloudDir();
    else
    {
        hideAllWindows();
        //pCloudWin->setCurrntIndxPclWin(DRIVE_PAGE_NUM);
        this->showWindow(pCloudWin);
    }
}

void PCloudApp::showSync()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(SYNC_PAGE_NUM);
    pCloudWin->get_sync_page()->openTab(0);
    this->showWindow(pCloudWin);
}

void PCloudApp::showSyncSttngs()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(SYNC_PAGE_NUM);
    pCloudWin->get_sync_page()->openTab(1);
    this->showWindow(pCloudWin);
}

void PCloudApp::showShares()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(SHARES_PAGE_NUM);
    this->showWindow(pCloudWin);
}

void PCloudApp::showCrypto()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(CRYPTO_PAGE_NUM);
    this->showWindow(pCloudWin);
}

void PCloudApp::showSettings()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(SETTINGS_PAGE_NUM);
    this->showWindow(pCloudWin);
}

void PCloudApp::showpcloudHelp()
{
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(HELP_PAGE_NUM);
    this->showWindow(pCloudWin);
}

void PCloudApp::showpCloudAbout(){
    hideAllWindows();
    pCloudWin->setCurrntIndxPclWin(ABOUT_PAGE_NUM);
    this->showWindow(pCloudWin);
}

const QPoint PCloudApp::calcWinNextToTrayCoords(const int winWidth, const int winHeigh)
{
    QDesktopWidget *desktop = QApplication::desktop();
    int xres, yres, avlbGeomW = desktop->availableGeometry().width();
    QPoint trayBL = this->tray->geometry().bottomLeft();
    if (trayBL.x() <2 || trayBL.y()<2)
        return QPoint (desktop->availableGeometry().bottomRight().rx()-winWidth, desktop->availableGeometry().bottomRight().ry()-winHeigh); //default location

    QPoint avlbGeomTL = desktop->availableGeometry().topLeft();
    int trayx = trayBL.x(), trayy = trayBL.y();

    //qDebug()<<"calcWinNextToTrayCoords" << trayBL << avlbGeomTL<<"menu"<<loggedmenu->geometry().topLeft()<<"tray coords"<< trayx<<trayy << winHeigh;

    //calc x
    if(trayx < avlbGeomW/2) // I or IV quadrant (left vertical half of the screen)
    {
        xres = qMax(trayx, avlbGeomTL.x());
    }
    else //II or III quadrant
    {
        xres = qMin(trayx,avlbGeomW - winWidth);
    }

    //calc y
    if(trayy < desktop->availableGeometry().height()/2) //I or II quadrant
    {
        yres = qMax(trayy, desktop->availableGeometry().left());
    }
    else // III or IV quadrant
    {
        yres = qMin(trayy, desktop->availableGeometry().bottom() - winHeigh);
    }

    //qDebug()<<"calcWinNextToTrayCoords res"<<xres<<yres;
    return QPoint(xres,yres);

}

void PCloudApp::openCloudDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(psync_fs_getmountpoint()));

    /*p    QString path = settings->get("path");

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
*/
}


void PCloudApp::logOut(){
    loggedin=false;

#ifdef Q_OS_WIN
    /*p
    if (notifythread){
        notifythread->terminate();
        notifythread->wait();
        delete notifythread;
        notifythread = NULL;
    }
*/

#endif
    username="";
    if(this->lastStatus != PSTATUS_BAD_LOGIN_DATA || this->lastStatus != PSTATUS_LOGIN_REQUIRED || this->lastStatus != PSTATUS_BAD_LOGIN_TOKEN)
        psync_logout();
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    pCloudWin->setOnlineItems(false);
    emit changeSyncIcon(0);
    this->hideAllWindows();
    if(!this->unlinkFlag)
        this->isFirstLaunch = false;
    notificationsMngr->clear();
    this->showLogin();
    //p unmount
}

void PCloudApp::unlink()
{
    psync_unlink();
    unlinkFlag = true; //init the sync gui part, remove synced folder menu etc.
    if(isLogedIn()) // when unlink come from the pcloudwin and the user was logged
        emit this->logOut(); //sets offline gui items also
    setFirstLaunch(true);   // to show suggestions
    if(noFreeSpaceMsgShownFlag)
        noFreeSpaceMsgShownFlag = false;
    removeSetting("autostartcrypto");
    //clearAllSettings();
    //stopfs
}

void PCloudApp::removeSetting(QString settingKey)
{
    if(settings->contains(settingKey))
        settings->remove(settingKey);
}
void PCloudApp::clearUpdtNotifctnSettngs()
{
    removeSetting("vrsnNotifyInvervalIndx");
}
void PCloudApp::clearAllSettings()
{
    clearUpdtNotifctnSettngs();

    //clear settings from settings page
#ifdef Q_OS_WIN
    removeSetting("shellExt");
    if(!registrySttng->contains("pCloud")) //set app to auto start with  windows; this setting is written in windows registry
    {
        QSettings appDir("HKEY_LOCAL_MACHINE\\SOFTWARE\\PCloud\\pCloud",QSettings::NativeFormat); //take app install ddir
        registrySttng->setValue("pCloud",appDir.value("Install_Dir").toString().append("\\pCloud.exe"));
    }
#endif

}

void PCloudApp::doExit(){
    //p unMount();
    // psync_destroy();
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
    {
        if(this->newNtfFLag)
        {
            notificationsMngr->showNotificationsWin();
            this->newNtfFLag = false;
        }
        else
            this->openCloudDir();
    }
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
    // qDebug()<<Q_FUNC_INFO<<"tray activation reason"<<reason;
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::MiddleClick) //3 = Trigger - left click
        showOnClick();
}

void PCloudApp::createMenus()
{
    //NOTLOGGED MENU
    notloggedmenu=new QMenu();
    registerAction=new QAction(QIcon(":/menu/images/menu 48x48/register.png"),trUtf8 ("Register"), this);
    connect(registerAction, SIGNAL(triggered()), this, SLOT(showRegister()));
    loginAction=new QAction(QIcon(":/menu/images/menu 48x48/login.png"),trUtf8("Login"), this);
    connect(loginAction, SIGNAL(triggered()), this, SLOT(showLogin()));
    helpAction = new QAction(QIcon(":/menu/images/menu 32x32/help.png"),trUtf8("Help"),this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showpcloudHelp()));
    aboutPCloudAction = new QAction(QIcon(":/menu/images/menu 32x32/info.png"),trUtf8("About"), this);
    connect(aboutPCloudAction, SIGNAL(triggered()), this, SLOT(showpCloudAbout()));
    exitAction=new QAction(QIcon(":/menu/images/menu 48x48/exit.png"),trUtf8("Exit"), this); // to be hidden in account tab
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));

    notloggedmenu->addAction(registerAction);
    notloggedmenu->addAction(loginAction);
    notloggedmenu->addSeparator();
    notloggedmenu->addAction(helpAction);
    notloggedmenu->addAction(aboutPCloudAction);
    notloggedmenu->addSeparator();
    notloggedmenu->addAction(exitAction);

    //LOGGED MENU
    //main menu actions
    accountAction = new QAction(QIcon(":/menu/images/menu 32x32/user.png"),trUtf8("Account"), this); // Account tab
    connect(accountAction, SIGNAL(triggered()),this, SLOT(showAccount()));
    userinfoAction = new QAction(QIcon(":/menu/images/menu16x16/spaceinfo.png"), "", this); //UserInfo - space action
    driveAction = new QAction(QIcon(":/menu/images/menu16x16/drive.png"),trUtf8("Open Drive"), this); //pDrive tab
    //connect(driveAction, SIGNAL(triggered()), this, SLOT(showDrive()));
    connect(driveAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));

    //crypto
    cryptoWelcomeAction = new QAction(QIcon(":/menu/images/menu16x16/crypto.png"),trUtf8("Crypto"), this); //Crypto tab
    connect(cryptoWelcomeAction, SIGNAL(triggered()), this, SLOT(showCrypto()));
    cryptoFldrLockedAction = new QAction(QIcon(":/menu/images/menu16x16/crypto-unlck.png"),trUtf8("Unlock Crypto"), this);
    connect(cryptoFldrLockedAction, SIGNAL(triggered()), this, SLOT(unlockCryptoFldr()));
    cryptoFldrUnlockedAction =  new QAction(QIcon(":/menu/images/menu16x16/crypto.png"),trUtf8("Lock Crypto"), this);
    connect(cryptoFldrUnlockedAction, SIGNAL(triggered()), this, SLOT(lockCryptoFldr()));
    cryptoOpenFldrAction = new QAction(QIcon(":/menu/images/menu16x16/cryptoFldr.png"),trUtf8("Open Folder"),this);
    connect(cryptoOpenFldrAction, SIGNAL(triggered()), this, SLOT(openCryptoFldr()));

    notfctnsAction = new QAction (QIcon(":/32x32/images/32x32/notifications.png"),trUtf8("Notifications"), this); // to update Icon
    connect(notfctnsAction, SIGNAL(triggered()), notificationsMngr, SLOT(showNotificationsWin()));
    settingsAction=new QAction(QIcon(":/menu/images/menu 32x32/settings.png"),trUtf8("Settings"), this); //Settings tab
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    pauseSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/pause.png"),trUtf8("Pause"),this);
    connect(pauseSyncAction, SIGNAL(triggered()),this,SLOT(pauseSync()));
    resumeSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/resume.png"),trUtf8("Resume"), this);
    connect(resumeSyncAction, SIGNAL(triggered()), this, SLOT(resumeSync()));
    syncDownldAction = new QAction(QIcon(":/menu/images/menu 48x48/download.png"),trUtf8("Everything downloaded"),this);
    syncUpldAction = new QAction(QIcon(":/menu/images/menu 48x48/upload.png"),trUtf8("Everything uploaded"),this);

    //sync actions
    syncAction = new QAction(QIcon(":/menu/images/menu16x16/manage.png"),trUtf8("Manage"),this);
    connect(syncAction, SIGNAL(triggered()), this, SLOT(showSync()));
    addSyncAction = new QAction(QIcon(":/menu/images/menu 48x48/newsync.png"),trUtf8("Add New"),this);
    connect(addSyncAction, SIGNAL(triggered()),this, SLOT(addNewSync()));
    connect(this, SIGNAL(addNewSyncSgnl()), this, SLOT(addNewSync()));
    connect(this, SIGNAL(addNewSyncLstSgnl(bool)), this, SLOT(addNewSyncLst(bool)));  //for creating syncs from OS file browser Contextmenu
    syncSttngsAction = new QAction(QIcon(":/menu/images/menu 32x32/settings.png"),trUtf8("Settings"),this); // may be to del
    connect(syncSttngsAction, SIGNAL(triggered()), this, SLOT(showSyncSttngs()));

    //shares actions
    sharesAction = new QAction(QIcon(":/menu/images/menu16x16/manage.png"),trUtf8("Manage"),this);
    connect(sharesAction, SIGNAL(triggered()), this, SLOT(showShares()));
    shareFolderAction = new QAction(QIcon(":/menu/images/menu 48x48/newsync.png"), trUtf8("Add New"),this);
    connect(shareFolderAction, SIGNAL(triggered()), this, SLOT(addNewShare()));

    //create tray menu and it's submenus and add actions
    loggedmenu = new QMenu();
    loggedmenu->addAction(driveAction);
    loggedmenu->addAction(cryptoWelcomeAction);
    loggedmenu->addAction(cryptoFldrLockedAction);
    cryptoUnlockedMenu = new QMenu(trUtf8("Crypto"));
    cryptoUnlockedMenu->setIcon(QIcon(":/menu/images/menu16x16/crypto.png"));
    cryptoUnlockedMenuAction = loggedmenu->addMenu(cryptoUnlockedMenu);
    cryptoUnlockedMenu->addAction(cryptoOpenFldrAction);
    cryptoUnlockedMenu->addAction(cryptoFldrUnlockedAction);
    loggedmenu->addSeparator();

    syncMenu = loggedmenu->addMenu(QIcon(":/menu/images/menu 32x32/sync.png"),trUtf8("Sync"));
    syncedFldrsMenu = syncMenu->addMenu(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),trUtf8("Synced Folders"));
    // this->createSyncFolderActions(); //loads sub menu with local synced folders
    syncMenu->addSeparator();
    syncMenu->addAction(syncAction);
    syncMenu->addAction(addSyncAction);

    sharesMenu = loggedmenu->addMenu(QIcon(":/menu/images/menu 32x32/share.png"),trUtf8("Shares"));
    sharesMenu->addAction(sharesAction);
    sharesMenu->addAction(shareFolderAction);

    loggedmenu->addSeparator();
    loggedmenu->addAction(notfctnsAction);
    loggedmenu->addAction(settingsAction);
    loggedmenu->addAction(accountAction);
    loggedmenu->addAction(userinfoAction);
    loggedmenu->addAction(helpAction);
    loggedmenu->addAction(aboutPCloudAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(exitAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(pauseSyncAction);
    loggedmenu->addAction(resumeSyncAction);
    loggedmenu->addAction(syncUpldAction);
    loggedmenu->addAction(syncDownldAction);

    pstatus_t status;
    memset(&status, 0, sizeof(status));
    psync_get_status(&status);
    if (status.status != PSTATUS_PAUSED)
    {
        resumeSyncAction->setVisible(false);
        //pCloudWin->ui->btnResumeSync->setVisible(false);
    }
    else
    {
        pauseSyncAction->setVisible(false);
        // pCloudWin->ui->btnPauseSync->setVisible(false);
    }

    connect(loggedmenu, SIGNAL(aboutToShow()), this, SLOT(refreshTray()));
    connect(syncedFldrsMenu, SIGNAL(aboutToShow()),this,SLOT(createSyncFolderActions())); //delete old if exist, adds new
    //connect(syncMenu, SIGNAL(aboutToShow()),this,SLOT(createSyncFolderActions())); //fu4ur version
    userinfoAction->setEnabled(false);
    syncDownldAction->setEnabled(false);
    syncUpldAction->setEnabled(false);

#ifdef Q_OS_WIM
    dbgPipeHlprActn = new QAction("Debug Pipe",this); //TEMP
    connect(dbgPipeHlprActn, SIGNAL(triggered()), this, SLOT(dbgPipeHlprSLot()));
    loggedmenu->addAction(dbgPipeHlprActn);
#endif

}
#ifdef Q_OS_WIM
#define  PIPE_NAME L"\\\\.\\pipe\\shellextnpipe2"

void PCloudApp::dbgPipeHlprSLot()
{
    HANDLE hPipedbg;

    //open & create
    hPipedbg = CreateFile(
                PIPE_NAME,
                GENERIC_WRITE | GENERIC_READ, // read and write access
                0,              // no sharing
                NULL,           // default security attributes
                OPEN_EXISTING,  // opens existing pipe
                FILE_ATTRIBUTE_NORMAL, // default attributes
                NULL);


    if (hPipedbg == INVALID_HANDLE_VALUE)
    {
        if(GetLastError() == ERROR_PIPE_BUSY)
        {
            if (!WaitNamedPipe(PIPE_NAME, 10000))
            {
                OutputDebugString(L"Qt dbgpipe: ERR_PIPE_BUSY - Could not open pipe: 10 second wait timed out.");
                return;
            }
        }
    }
    //write
    char buffer[270];
    strcpy(buffer, "synclist*P:\\r(1)");
    // strcpy(buffer, "addsyncD:\\newwww|");
    //strcpy(buffer,"synclist");

    //_tcscpy_s()

    DWORD size = (strlen(buffer))*sizeof(char);

    bool result = WriteFile(
                hPipedbg,
                buffer, // the data to be sent
                (strlen(buffer)+1)*sizeof(char), //length of data to send
                &size, //actual amount of sent data
                NULL); // not using overlapped IO
    FlushFileBuffers(hPipedbg);

    if(!result)
        qDebug()<<"Qt: dbgpipe write failed "<<GetLastError();

    // read


}
#endif

void status_callback(pstatus_t *status)
{
    mutex.lock();
    quint32 err = psync_get_last_error();
    if(err)
        qDebug()<<"status callback get last error: "<<err;

    // ++ syncing flag
    quint32 previousStatus = PCloudApp::appStatic->lastStatus;
    switch(status->status)
    {
    case PSTATUS_READY:                     //0
        qDebug()<<"PSTATUS_READY" << status->downloadstr << status->uploadstr;
        if (previousStatus != PSTATUS_READY) //
        {
            if(PCloudApp::appStatic->isLogedIn())
            {
                PCloudApp::appStatic->changeSyncIconPublic(1);
                if(PCloudApp::appStatic->nointernetFlag)
                {
                    PCloudApp::appStatic->changeOnlineItemsPublic(true);
                    PCloudApp::appStatic->nointernetFlag = false; //already connected to net
                }
            }

            if(previousStatus == PSTATUS_DOWNLOADING || previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_PAUSED)
                //PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything Downloaded");
                PCloudApp::appStatic->downldInfo = status->downloadstr;
            if(previousStatus == PSTATUS_UPLOADING || previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_PAUSED)
                //PCloudApp::appStatic->uploadInfo = QObject::tr("Everything Uploaded");
                PCloudApp::appStatic->uploadInfo = status->uploadstr;

            if (PCloudApp::appStatic->isMenuorWinActive())
                PCloudApp::appStatic->updateSyncStatusPublic();

            if(PCloudApp::appStatic->noFreeSpaceMsgShownFlag)
                PCloudApp::appStatic->noFreeSpaceMsgShownFlag = false;

            PCloudApp::appStatic->lastStatus = PSTATUS_READY;
        }
        break;

    case PSTATUS_DOWNLOADING:               //1
        qDebug()<<"PSTATUS_DOWNLOADING"<<
                  "bytes downloaded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed
               <<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload<<"is full: "<< status->localisfull<<status->remoteisfull;

        if(!PCloudApp::appStatic->isLogedIn())
            break;

        if(!(previousStatus == PSTATUS_DOWNLOADING || previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_UPLOADING))
            PCloudApp::appStatic->changeSyncIconPublic(2);

        PCloudApp::appStatic->downldInfo = status->downloadstr;

        /* move text building to lib
        if (PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_DOWNLOADING)
        {
            if (status->bytestodownload)
            {
                char files[16];
                if (status->filestodownload > 1)
                    strcpy(files, " files");
                else
                    strcpy(files, " file");

                if(status->downloadspeed)// sometimes lib returns 0
                {
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ") + QString::number(status->downloadspeed/1000) + " kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestodownload/status->downloadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + files;
                }
                else
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ")  + PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) +
                            ", " +    QString::number(status->filestodownload) + files;
            }
            else
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything Downloaded");
                }
        */

        if(previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_UPLOADING || previousStatus == PSTATUS_PAUSED) //case when come upload just has finished
            PCloudApp::appStatic->uploadInfo = status->uploadstr;

        if(PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_DOWNLOADING)
            PCloudApp::appStatic->updateSyncStatusPublic();

        PCloudApp::appStatic->lastStatus = PSTATUS_DOWNLOADING;
        break;

    case PSTATUS_UPLOADING:                 //2
        qDebug()<<"PSTATUS_UPLOADING";
        if(!PCloudApp::appStatic->isLogedIn())
            break;

        if(!(previousStatus == PSTATUS_DOWNLOADING || previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_UPLOADING))
            PCloudApp::appStatic->changeSyncIconPublic(2);
        qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed
               <<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload<<"is full: "<< status->localisfull<<status->remoteisfull;

        PCloudApp::appStatic->uploadInfo = status->uploadstr;

        /* string building moved to lib
        if (PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_UPLOADING)
        {
            if (status->bytestoupload)
            {
                char files[16];
                if (status->filestoupload > 1)
                    strcpy(files, " files");
                else
                    strcpy(files, " file");

                if(status->uploadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Upload: ") + QString::number(status->uploadspeed/1000) + " kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestoupload/status->uploadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + files;
                }
                else
                    PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Upload: ")  + PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + files;
            }
            else
                PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Everything Uploaded");

            //case when download just has finished
            if(previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_DOWNLOADING)
                PCloudApp::appStatic->downldInfo = status->downloadstr;

            PCloudApp::appStatic->updateSyncStatusPublic();
        }
        */

        //case when download just has finished
        if(previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING || previousStatus == PSTATUS_DOWNLOADING || previousStatus == PSTATUS_PAUSED)
            PCloudApp::appStatic->downldInfo = status->downloadstr;

        if(PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_UPLOADING)
            PCloudApp::appStatic->updateSyncStatusPublic();

        PCloudApp::appStatic->lastStatus = PSTATUS_UPLOADING;
        break;

    case PSTATUS_DOWNLOADINGANDUPLOADING:   //3
        qDebug()<<"PSTATUS_DOWNLOADINGANDUPLOADING";

        if(!PCloudApp::appStatic->isLogedIn())
            break;

        if(!(previousStatus == PSTATUS_DOWNLOADING || previousStatus == PSTATUS_DOWNLOADINGANDUPLOADING
             || previousStatus == PSTATUS_UPLOADING))
            PCloudApp::appStatic->changeSyncIconPublic(2);

        qDebug()<<"DOWNLOAD bytes downlaoded "<<status->bytesdownloaded << "bytestodownload= "<<status->bytestodownload << " current "<<status->bytestodownloadcurrent<< " speed" <<status->downloadspeed
               <<"DOWNLOAD files filesdownloading "<<status->filesdownloading<< " filestodownload="<<status->filestodownload<<"is full: "<< status->localisfull<<status->remoteisfull;

        qDebug()<<"UPLOAD bytes    bytesuploaded=  "<<status->bytesuploaded << " bytestoupload = "<<status->bytestoupload << " current= "<<status->bytestouploadcurrent<<" speed" <<status->uploadspeed
               <<"UPLOAD filesuploading=  "<<status->filesuploading<< " filestoupload= "<<status->filestoupload;

        PCloudApp::appStatic->downldInfo = status->downloadstr;
        PCloudApp::appStatic->uploadInfo= status->uploadstr;

        if (PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_DOWNLOADINGANDUPLOADING)
            PCloudApp::appStatic->updateSyncStatusPublic();

        /*
        if (PCloudApp::appStatic->isMenuorWinActive() || previousStatus != PSTATUS_DOWNLOADINGANDUPLOADING)
        {
            if(status->bytestodownload)
            {
                char filesdwnld[16];
                if (status->filestodownload > 1)
                    strcpy(filesdwnld, " files");
                else
                    strcpy(filesdwnld, " file");

                if(status->downloadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ") + QString::number(status->downloadspeed/1000) + " kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestodownload/status->downloadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + filesdwnld;
                }
                else
                    PCloudApp::appStatic->downldInfo = QObject::trUtf8("Download: ")  + PCloudApp::appStatic->bytesConvert(status->bytestodownload - status->bytesdownloaded) + ", " +
                            QString::number(status->filestodownload) + filesdwnld;
            }
            else
                PCloudApp::appStatic->downldInfo = QObject::trUtf8("Everything Downloaded");


            if(status->bytestoupload)
            {

                char filesupld[16];
                if (status->filestoupload > 1)
                    strcpy(filesupld, " files");
                else
                    strcpy(filesupld, " file");

                if(status->uploadspeed)// sometimes is 0
                {
                    PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Upload: ") + QString::number(status->uploadspeed/1000) + " kB/s, " +
                            PCloudApp::appStatic->timeConvert(status->bytestoupload/status->uploadspeed) + ", " +
                            PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) + ", " +
                            QString::number(status->filestoupload) + filesupld;
                }
                else
                    PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Upload: ")  + PCloudApp::appStatic->bytesConvert(status->bytestoupload - status->bytesuploaded) +", " +
                            QString::number(status->filestoupload) + filesupld;
            }
            else
                PCloudApp::appStatic->uploadInfo = QObject::trUtf8("Everything Uploaded");
            PCloudApp::appStatic->updateSyncStatusPublic();
        }
        */

        PCloudApp::appStatic->lastStatus = PSTATUS_DOWNLOADINGANDUPLOADING;
        break;

    case PSTATUS_LOGIN_REQUIRED:            //4
        qDebug()<<"PSTATUS_LOGIN_REQUIRED";
        if(PCloudApp::appStatic->isLogedIn() && previousStatus != PSTATUS_LOGIN_REQUIRED)
            PCloudApp::appStatic->logoutPublic();
        PCloudApp::appStatic->lastStatus = PSTATUS_LOGIN_REQUIRED;
        break;

    case PSTATUS_BAD_LOGIN_DATA:            //5
        qDebug()<<"PSTATUS_BAD_LOGIN_DATA";
        if(previousStatus != PSTATUS_BAD_LOGIN_DATA && PCloudApp::appStatic->isLogedIn())
            PCloudApp::appStatic->logoutPublic();
        PCloudApp::appStatic->lastStatus = PSTATUS_BAD_LOGIN_DATA;
        break;

    case PSTATUS_BAD_LOGIN_TOKEN: //6
        qDebug()<<"PSTATUS_BAD_LOGIN_TOKEN";
        if(previousStatus != PSTATUS_BAD_LOGIN_TOKEN)
        {
            if (PCloudApp::appStatic->isLogedIn())
                PCloudApp::appStatic->logoutPublic();
            PCloudApp::appStatic->lastStatus = PSTATUS_BAD_LOGIN_TOKEN;
            PCloudApp::appStatic->showMsgBoxPublic("Session expired", "Your session has expired. Please login again.", QMessageBox::Critical);
        }
        break;

    case PSTATUS_ACCOUNT_FULL:              //7
        qDebug()<<"PSTATUS_ACCOUNT_FULL";
        if(previousStatus != PSTATUS_ACCOUNT_FULL)
        {
            PCloudApp::appStatic->lastStatus = PSTATUS_ACCOUNT_FULL;
            PCloudApp::appStatic->changeSyncIconPublic(4);
        }
        break;

    case PSTATUS_DISK_FULL:
        qDebug()<<"PSTATUS_DISK_FULL";
        if(previousStatus != PSTATUS_DISK_FULL)
        {
            PCloudApp::appStatic->lastStatus = PSTATUS_DISK_FULL;
            PCloudApp::appStatic->changeSyncIconPublic(4);
            if (PCloudApp::appStatic->isLogedIn())
                PCloudApp::appStatic->changeOnlineItems(true);
        }
        break;

    case PSTATUS_PAUSED:
        qDebug()<<"PSTATUS_PAUSED";
        if (PCloudApp::appStatic->isLogedIn() && previousStatus != PSTATUS_PAUSED)
            PCloudApp::appStatic->changeSyncIconPublic(3);
        PCloudApp::appStatic->downldInfo = status->downloadstr;
        PCloudApp::appStatic->uploadInfo = status->uploadstr;
        PCloudApp::appStatic->lastStatus = PSTATUS_PAUSED;
        //update menu -> start sync for initial login
        break;

    case PSTATUS_STOPPED:
        qDebug()<<"PSTATUS_STOPPED";
        PCloudApp::appStatic->changeSyncIconPublic(0);
        PCloudApp::appStatic->lastStatus = PSTATUS_STOPPED;
        break;

    case PSTATUS_OFFLINE:
        qDebug()<<"PSTATUS_OFFLINE";
        if(previousStatus != PSTATUS_OFFLINE)
        {
            PCloudApp::appStatic->changeSyncIconPublic(0);
            //PCloudApp::appStatic->changeOnlineItemsPublic(false);
            PCloudApp::appStatic->nointernetFlag = true;
            PCloudApp::appStatic->lastStatus = PSTATUS_OFFLINE;
        }
        break;

    case PSTATUS_CONNECTING:
        qDebug()<<"PSTATUS_CONNECTING";
        PCloudApp::appStatic->lastStatus = PSTATUS_CONNECTING;
        break;

    case PSTATUS_SCANNING:
        qDebug()<<" PSTATUS_SCANNING";
        PCloudApp::appStatic->lastStatus = PSTATUS_SCANNING;
        break;

    case PSTATUS_USER_MISMATCH:
        //case when set wrong user
        qDebug()<<"PSTATUS_USER_MISMATCH";
        PCloudApp::appStatic->lastStatus = PSTATUS_USER_MISMATCH;
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
    if(PCloudApp::appStatic->noEventCallbackFlag)
    {
        PCloudApp::appStatic->noEventCallbackFlag = false;
        return;
    }
    char title[128], msg[512];
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
        qDebug()<<"PEVENT_SHARE_REQUESTIN"; // someone else shares me a folder, can be added from web
        strcpy(title, "New Share Request Received!");
        strcpy(msg,"You received a new Share Request from \"");
        strcat(msg, data.share->email);
        strcat(msg,"\"");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,1);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM)) //if shraes page is visible
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
        break;
    case PEVENT_SHARE_REQUESTOUT:
        qDebug()<<"PEVENT_SHARE_REQUESTOUT" <<data.share->email<< data.share->sharename; // i share a folder 1.1
        strcpy(title, "Share Request Sent Successfully!");
        strcpy(msg,"You successfully sent a Share Request \"");
        strcat(msg, data.share->sharename);
        strcat(msg," to \"");
        strcat(msg, data.share->email);
        strcat(msg,"\"");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,0);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
        break;
    case PEVENT_SHARE_ACCEPTIN:
        qDebug()<<"PEVENT_SHARE_ACCEPTIN"; //2.2 I accept a share - refresh both tables in Shared with me
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
        break;
    case PEVENT_SHARE_ACCEPTOUT: // when someones accept what i've shared to him
        qDebug()<<"PEVENT_SHARE_ACCEPTOUT";
        strcpy(title, "Share Request Accepted!");
        strcpy(msg,"\"");
        strcat(msg,data.share->email);
        strcat(msg, "\" accepted your Share Request \"");
        strcat(msg, data.share->sharename);
        strcat(msg,"\"");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,0);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
        break;
    case PEVENT_SHARE_DECLINEIN: // reject a share request 2,2
        qDebug()<<"PEVENT_SHARE_DECLINEIN";
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
        break;
    case PEVENT_SHARE_DECLINEOUT:
        qDebug()<<"PEVENT_SHARE_DECLINEOUT"; //when someones rejected what i've shared to him
        strcpy(title, "Share Request Declined!");
        strcpy(msg,"\"");
        strcat(msg,data.share->email);
        strcat(msg, "\" declined your Share Request \"");
        strcat(msg, data.share->sharename);
        strcat(msg,"\"");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,0);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
        break;
    case PEVENT_SHARE_CANCELIN:
        qDebug()<<"PEVENT_SHARE_CANCELIN";
        // some one send me a request and HE stopped the request before i choose what to do with it
        strcpy(title,"Share Request Canceled!");
        strcpy(msg, "\"");
        strcat(msg,data.share->email);
        strcat(msg, "\" canceled his/her Share Request");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,1);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
        break;
    case PEVENT_SHARE_CANCELOUT:
        qDebug()<<"PEVENT_SHARE_CANCELOUT"; // stop a my request - 1,2
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
        break;
    case PEVENT_SHARE_REMOVEIN: // when I stops a request that was send to me and i have been accepted it,  - my requests 2,1
        // two cases also
        qDebug()<<"PEVENT_SHARE_REMOVEIN";
        /* temp till get the flag
        *  strcpy(title,"Share Stopped");
        //add " for email
        strcpy(msg,data.share->email);
        strcat(msg, " has stopped your access to ");
        strcat(msg,data.share->sharename);
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,1);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
            */

        break;
    case PEVENT_SHARE_REMOVEOUT: // stop my share - 1,1
        qDebug()<<"PEVENT_SHARE_REMOVEOUT";
        //TEmp commented till get the flag
        //case1:  someoned has accepted and after that stopped what i've shared to him
        //case2 : i stop my share
        /*strcpy(title,"Share Stopped");
        strcpy(msg,data.share->email);
        strcat(msg, " has stopped his/her access to ");
        strcat(msg,data.share->sharename);
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,0);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
            */
        break;
    case PEVENT_SHARE_MODIFYIN:
        qDebug()<<"PEVENT_SHARE_MODIFYIN"; // some one shared me smthn and changes the permissions
        strcpy(title,"Share Modified!");
        strcpy(msg,"The Share \"");
        strcat(msg,data.share->sharename);
        strcat(msg,"\" has been modified by \"");
        strcat(msg,data.share->email);
        strcat(msg,"\"");
        PCloudApp::appStatic->sendTrayMsgTypePublic(title,msg,1);
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,1);
        break;
    case PEVENT_SHARE_MODIFYOUT:
        qDebug()<<"PEVENT_SHARE_MODIFYOUT"; // i change permissions
        if(PCloudApp::appStatic->isMainWinPageActive(SHARES_PAGE_NUM))
            PCloudApp::appStatic->pCloudWin->refreshPagePulbic(SHARES_PAGE_NUM,0);
        break;
    default:
        break;
    }
    mutex.unlock();
}

void notification_callback(quint32 notificationcnt, quint32 newnotificationcnt)
{
    mutex.lock();
    qDebug()<<"notification_callback"<<notificationcnt<<newnotificationcnt;
    PCloudApp::appStatic->updateNotfctnsPublic(newnotificationcnt);
    mutex.unlock();
}

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{
    /*p
#ifdef Q_OS_WIN
    notifythread = NULL;
#endif */

    ////setLayoutDirection(tr("LTR")=="RTL" ? Qt::RightToLeft : Qt::LeftToRight); //арабик фучур
    appStatic = this;
    fontPointSize = this->font().pointSize();
    smaller1pFont.setPointSize(fontPointSize - 1);
    smaller2pFont.setPointSize(fontPointSize - 2);
    bigger1pFont.setPointSize(fontPointSize + 1);
    bigger2pFont.setPointSize(fontPointSize + 2);
    bigger3pFont.setPointSize(fontPointSize + 3);
    regwin=NULL;
    logwin=NULL;
    loggedmenu=NULL;
    sharefolderwin = NULL;
    welcomeWin = NULL;
    syncFldrsWin = NULL;
    introwin = NULL;
    isFirstLaunch = false;
    isCryptoExpired = true;
    //p mthread=NULL;
    loggedin=false;
    newNtfFLag = false;
    lastMessageType=-1;
    //settings=new PSettings(this);
    settings=new QSettings("pCloud","pCloud");
    noFreeSpaceMsgShownFlag = false;
    noEventCallbackFlag = false;
    nointernetFlag = false;
    bytestoDwnld = 0;
    bytestoUpld = 0;
    downldInfo = QObject::trUtf8("Everything Downloaded");
    uploadInfo = QObject::trUtf8("Everything Uploaded");
    unlinkFlag = false;
    isCursorChanged = false;

    getDeskopEnv();
    lastStatus = PSTATUS_CONNECTING;
    tray=new QSystemTrayIcon(QIcon(OFFLINE_ICON),this);
    this->lastTrayIconIndex = 0;
    connect(this, SIGNAL(sendTrayMsgType(const char*,const char*,int)),
            this, SLOT(showTrayMsgType(const char*,const char*,int)));
    connect(this, SIGNAL(showMsgBoxSgnl(QString,QString,int)), this, SLOT(showMsgBox(QString,QString,int)));
    this->setQuitOnLastWindowClosed(false); // if this is true app will close on every shown message with no shown parent
#if defined(Q_OS_LINUX) && QT_VERSION<QT_VERSION_CHECK(5,0,0)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")); // for non-latin strings
#endif
    tray->setToolTip("pCloud");
    tray->show();
    /*
    if (psync_init() == -1)
    {
        QMessageBox::critical(NULL, "pCloud Drive", tr("pCloud Drive has stopped. Please connect to our support"));
        qDebug()<<" psync-init returned -1 "<<psync_get_last_error();
        this->quit();
    }
*/

    psync_set_notification_callback(notification_callback,"48x48"); //have to be called between init and start
    psync_start_sync(status_callback,event_callback); // if not started from context menu ++
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(;;)
    {
        pstatus_t status;
        memset(&status, 0, sizeof(status));
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
    updateNtfctnTimer = new QTimer(this);
    check_version(); // call before pcldwin to be created because of about page content
#ifdef Q_OS_WIN
    registrySttng = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat); //needed for pcloudwin
    shellExtThread = new ShellExtThread(this);
    shellExtThread->start();
#endif
    this->setWheelScrollLines(1);
    pCloudWin = new PCloudWindow(this);  //needs settings to be created
    pCloudWin->layout()->setSizeConstraint(QLayout::SetFixedSize); //for auto resize
    pCloudWin->setOnlineItems(false);
    notificationsMngr = new NotificationsManager(this);
    createMenus(); //needs sync to be started
    tray->setContextMenu(notloggedmenu);
    connect(loggedmenu, SIGNAL(triggered(QAction*)), this, SLOT(refreshTray()));
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(trayMsgClicked()));
    connect(this, SIGNAL(showLoginSgnl()),this, SLOT(showLogin()));
    connect(this, SIGNAL(logoutSignl()), this, SLOT(logOut()));
    connect(this, SIGNAL(changeSyncIcon(int)), this, SLOT(setTrayIcon(int)));
    connect(this, SIGNAL(changeOnlineItemsSgnl(bool)), this, SLOT(changeOnlineItems(bool)));
    connect(this, SIGNAL(changeCursor(bool)), this, SLOT(setCursor(bool)));
    connect(this, SIGNAL(sendErrText(int, const char*)), this, SLOT(setErrText(int,const char*)));
    connect(this, SIGNAL(updateSyncStatusSgnl()), this, SLOT(updateSyncStatus()));
    connect(this, SIGNAL(refreshSyncUIitemsSgnl()), this, SLOT(refreshSyncUIitems()));
    connect(this, SIGNAL(updateUserInfoSgnl(const char* &)), this, SLOT(updateUserInfo(const char* &)));
    connect(this, SIGNAL(addNewShareSgnl(QString)), this, SLOT(addNewShare(QString)));
    connect(this, SIGNAL(unlockCryptoFldrSgnl()), this, SLOT(unlockCryptoFldr()));
    connect(this, SIGNAL(lockCryptoFldrSgnl()), this, SLOT(lockCryptoFldr()));
    connect(this, SIGNAL(updateNotfctnsModelSgnl(int)), notificationsMngr, SLOT(updateNotfctnsModel(int)));
    bool savedauth = psync_get_bool_value("saveauth"); //works when syns is paused also

    qDebug()<<"saveauth"<<savedauth << "username" <<psync_get_username();
    //if (!savedauth)
    const char* auth = psync_get_auth_string();
    //if (!strcmp(psync_get_auth_string(), ""))
    if (!strcmp(auth, ""))
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

    /* p
        else
        othread=new OnlineThread(this);
        othread->start();
    }p */

}

PCloudApp::~PCloudApp(){
    qDebug()<<"destructing app";
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
    if(updateNtfctnTimer)
    {
        this->stopTimer();
        delete updateNtfctnTimer;
    }
#ifdef Q_OS_WIN
    if (shellExtThread)
    {
        if(shellExtThread->isRunning())
        {
            qDebug()<<"Qt: terminate shellthread";
            shellExtThread->terminate();
        }
        shellExtThread->wait();
        delete shellExtThread;
    }
#endif
    /*  if(versnThread)
    {
        if(versnThread->isRunning())
            versnThread->terminate();
        versnThread->wait();
        delete versnThread;
    }*/
    delete settings;
    delete tray;
    if (loggedmenu)
        delete loggedmenu;
    delete notloggedmenu;
    delete registerAction;
    delete loginAction;
    delete exitAction;
    delete settingsAction;
    delete sharesAction;
    delete cryptoWelcomeAction;
    delete cryptoFldrLockedAction;
    delete cryptoFldrUnlockedAction;
    delete cryptoOpenFldrAction;
    delete cryptoUnlockedMenuAction;
    delete userinfoAction;
    delete syncAction;
    delete helpAction;
    delete aboutPCloudAction;
    if (regwin)
        delete regwin;
    if (logwin)
        delete logwin;
    /*p if (settingswin)
        delete settingswin;*/
    if (pCloudWin)
        delete pCloudWin;
    if (welcomeWin)
        delete welcomeWin;
    if(syncFldrsWin)
        delete syncFldrsWin;
}

void PCloudApp::getDeskopEnv()
{
    QByteArray env;
    QProcess process;
    process.start("bash -c \"echo $DESKTOP_SESSION\"");
    process.waitForFinished(-1);
    env = process.readAllStandardOutput();
    if(!env.isNull())
    {
        if (env.toLower().contains("ubuntu"))
            desktopEnv = "ubuntu";
    }
    else
        desktopEnv = "";
}

void PCloudApp::check_error()
{
    quint32 err = psync_get_last_error();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("pCloud Drive");
    switch (err)
    {
    case PERROR_LOCAL_FOLDER_NOT_FOUND: //1
        qDebug()<<"PERROR_LOCAL_FOLDER_NOT_FOUND";
        break;
    case PERROR_REMOTE_FOLDER_NOT_FOUND: //2
        qDebug()<<"PERROR_REMOTE_FOLDER_NOT_FOUND";
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
        msgBox.setText(trUtf8("Can not add new sync: Folder already syncing!"));
        msgBox.exec();
        break;
    case PERROR_INVALID_SYNCTYPE:  //10
        qDebug()<< "PERROR_INVALID_SYNCTYPE";
        break;
    case PERROR_OFFLINE: //11
        msgBox.setText(trUtf8("Internal error!"));
        msgBox.setInformativeText(trUtf8("pCloud is offline now. Please reconnect"));
        msgBox.exec();
        break;
    case PERROR_INVALID_SYNCID: //12
        qDebug()<<"PERROR_INVALID_SYNCID";
        break;
    case PERROR_PARENT_OR_SUBFOLDER_ALREADY_SYNCING: //13
        msgBox.setText(trUtf8("Can not add new sync: Parent folder or subfolder of it has already synchronized!"));
        msgBox.setInformativeText(trUtf8("Please check your synchronized folders list"));
        msgBox.exec();
        break;
    case PERROR_LOCAL_IS_ON_PDRIVE: //14
        qDebug()<<"PERROR_LOCAL_IS_ON_PDRIVE";
        msgBox.setText(trUtf8("Can not add new sync: The selected local folder is from pDrive!"));
        msgBox.setInformativeText(trUtf8("Please select correct local folder"));
        msgBox.exec();
        break;
    default:
        break;
    }
}

void PCloudApp::showError(QString &err){
    tray->showMessage("Error", err, QSystemTrayIcon::Warning);
}

void PCloudApp::showTrayMessage(QString title, QString msg)
{
    tray->showMessage(title, msg, QSystemTrayIcon::Information);
}

void PCloudApp::logIn(const QString &uname, bool remember) //needs STATUS_READY
{
    if (this->unlinkFlag)
    {
        syncedFldrsMenu->clear();
        //this->clearSyncFolderActions(); //fu4ur version menu
        resumeSyncAction->setVisible(false);
        pauseSyncAction->setVisible(true);
        this->downldInfo = QObject::trUtf8("Everything Downloaded");
        this->uploadInfo = QObject::trUtf8("Everything Uploaded");
        //  this->pCloudWin->get_sync_page()->load();
        //this->pCloudWin->get_sync_page()->loadSettings();
        //this->pCloudWin->refreshPageSgnl(SETTINGS_PAGE_NUM, -1);
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

    userinfoAction->setText(this->usedSpaceStr + " ("+
                            QString::number(100 - this->freeSpacePercentage) + "%) of " + this->planStr + " used");

    switch (this->lastStatus)
    {
    case PSTATUS_ACCOUNT_FULL:
    case PSTATUS_DISK_FULL:
        emit changeSyncIcon(4);
        break;
    case PSTATUS_PAUSED:
        emit changeSyncIcon(3);
        break;
    case PSTATUS_OFFLINE:
        emit changeSyncIcon(0);
        break;
    default:
        emit changeSyncIcon(1);
    }

    pCloudWin->setOnlineItems(true);
    pCloudWin->initOnlinePages();
    tray->setContextMenu(loggedmenu);
    notificationsMngr->init();

    // isFirstLaunch = true; // for test TEMP
    /*
#ifdef Q_OS_WIN // NEXT VERSION - when screeshots for win are ready
    if (isFirstLaunch)
    {
        welcomeWin = new WelcomeWin(this, NULL);
        this->showWindow(welcomeWin);
    }
#else
*/
    //cases when: 1. After unlink 2. Show Welcomes again was left to be checked
    //3. update to 2.0.0+ from version < 2.0.0 but haven't unlink and have never seen Welcomes (Welcomes introduced in 2.0.0).
    if ((isFirstLaunch || (this->settings->contains("showintrowin") && this->settings->value("showintrowin").toBool()))
            || !this->settings->contains("welcomesNeverShowed"))
    {
        if(introwin == NULL)
            introwin = new InfoScreensWin(this);
        this->showWindow(introwin);

        if (!this->settings->contains("welcomesNeverShowed"))
        {
            this->settings->setValue("welcomesNeverShowed",true);
        }
    }

}

void PCloudApp::getUserInfo()
{    
    this->isVerified = psync_get_bool_value("emailverified");
    this->isPremium = psync_get_bool_value("premium");
    this->getQuota();
}

void PCloudApp::getQuota()
{
    quint64 quota = psync_get_uint_value("quota");
    if (quota)
    {
        this->planStr =  QString::number(quota >> 30 ) + " GB";
        quint64 usedquota =  psync_get_uint_value("usedquota");
        if (!usedquota) // sometimes  synclib doesn't return quaota immediately
        {
            int cnt = 0; //for empty account
            while (!usedquota && cnt < 5)
            {
                usedquota =  psync_get_uint_value("usedquota");
                cnt++;
                sleep(1);
            }
        }
        qDebug() << quota<< "used quota " << usedquota;
        if (usedquota)
        {
            if(usedquota > quota ) // bug at lib
                this->freeSpacePercentage = 0;
            else
                this->freeSpacePercentage = (100*(quota - usedquota))/quota; // should be only this line when bug is fixed

            this->usedSpaceStr = bytesConvert(usedquota);
        }
        else
        {
            this->usedSpaceStr = "0.00 GB";
            this->freeSpacePercentage = 100;
        }
    }
}

bool PCloudApp::getIsCryptoExpired()
{
    uint subscbtntTime = psync_crypto_expires();
    if  (!subscbtntTime || QDateTime::fromTime_t(subscbtntTime).addDays(30) < QDateTime::currentDateTime()
         || psync_crypto_issetup() == 0)
        this->isCryptoExpired = true;
    else
        this->isCryptoExpired = false;

    return this->isCryptoExpired;
}

void PCloudApp::trayMsgClicked()
{
    if(lastMessageType == -1 )
        return;

    if (lastMessageType == 3)
        emit showpCloudAbout();
    if (lastMessageType == 0 || lastMessageType == 1 )
    {
        emit showShares();
        pCloudWin->ui->tabWidgetShares->setCurrentIndex(lastMessageType);
        pCloudWin->sharesPage->refreshTab(lastMessageType);
    }
}
/*p
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
/*
void PCloudApp::check_version() // old specification
{
#ifdef Q_OS_LINUX
    (__WORDSIZE == 64)? OSStr = "LINUX64" : OSStr = "LINUX32";
#else
      if(QSysInfo::windowsVersion() != QSysInfo::WV_XP)
        OSStr = "WIN"; // downloads danny's installer
    else
    // end of comment
    OSStr = "WIN_XP";
#endif

    psync_new_version_t* version = psync_check_new_version_str(OSStr, APP_VERSION);
    if(version != NULL)
    {
        lastMessageType = 3;
        newVersionFlag = true;
        newVersion.notes = version->notes;
        newVersion.url = version->url;
        newVersion.versionstr = version->versionstr;

        qDebug()<<"new version"<<version->localpath<<version->updatesize;
        free(version);

        //the version reminder has been set to "Never" for the current version
        if(settings->contains("vrsnNotfyDtTime") &&
                (settings->value("vrsnNotfyDtTime").toDateTime().isNull() && (settings->value("lastAppNotifiedVrsn").toString() == APP_VERSION)))
        {
            qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (check_version: case 1): the version reminder has been set to Never for the current version ";
            versnThread = NULL;
            QTimer::singleShot(86400000, this, SLOT(check_version())); //check again for newer version after 24h
            return;
        }
        else // show notifications
        {
            //notifications settings for the current version are already has set, start timer with the time from settings
            if(settings->contains("vrsnNotfyDtTime") && !(settings->value("vrsnNotfyDtTime").toDateTime().isNull())) //if Reminder is already set (and is not "Never")
            {
                qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version: case 2):notifications settings for the current version are already has set";
                versnThread = new VersionTimerThread(this, settings->value("vrsnNotfyDtTime").toDateTime());
                versnThread->start();
            }
            else // notify for the new version for the first time (no notification settings set)
            {
                qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version: case 3) notify for the new version for the first time (no notification settings set)";
                tray->showMessage("New Version", "A new version of pCloud Sync is available!\nClick here for more details");
                //QDateTime timeForNotify = QDateTime::currentDateTime().addSecs(216000); //6h is the default
                QDateTime timeForNotify = QDateTime::currentDateTime().addSecs(30); //TEST
                settings->setValue("vrsnNotfyDtTime",timeForNotify);
                versnThread = new VersionTimerThread(this,timeForNotify);
                versnThread->start();
            }
        }
    }
    else
    {
        qDebug()<< QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version: case 4): no new version";
        newVersionFlag = false;
        versnThread = NULL;
        // clear notifications settings from before if exists
        if(QFile::exists(settings->fileName()))
        {
            clearUpdtNotifctnSettngs();
        }
        QTimer::singleShot(86400000, this, SLOT(check_version())); //check again after 24h
    }
}*/
void PCloudApp::check_version()
{
#ifdef Q_OS_LINUX
    (__WORDSIZE == 64)? OSStr = "LINUX64" : OSStr = "LINUX32";
#else
    /*  if(QSysInfo::windowsVersion() != QSysInfo::WV_XP)
        OSStr = "WIN"; // downloads danny's installer
    else
    */
    OSStr = "WIN_XP";
#endif
    psync_new_version_t* version = psync_check_new_version_str(OSStr, APP_VERSION);

    if(version != NULL) // a new version is available
    {
        lastMessageType = 3;
        newVersionFlag = true;
        newVersion.notes = version->notes;
        newVersion.url = version->url;
        newVersion.versionstr = version->versionstr;
        free(version);

        this->showPopupNewVersion();
        if(settings->contains("vrsnNotifyInvervalIndx")) // if notification interval was already selected on previous app launch
        {
            qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version: setTimerInterval):notifications time interval for the current version had already set";
            setTimerInterval(settings->value("vrsnNotifyInvervalIndx").toInt());
        }
        else
        {
            qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version: setTimerInterval):notifications time interval not set, use the default interval: 30 secs";
            setTimerInterval(0); //default val is one hour
        }
        connect(updateNtfctnTimer, SIGNAL(timeout()), this, SLOT(showPopupNewVersion()));
    }
    else
    {
        qDebug()<< QDateTime::currentDateTime() <<"NOTIFICATIONS (check_version): no new version - check again after 24h if app is still launced";
        newVersionFlag = false;

        if(QFile::exists(settings->fileName())) // clear notifications settings from before if exists
            clearUpdtNotifctnSettngs();
        QTimer::singleShot(86400000, this, SLOT(check_version())); //check again after 24h
    }
}
bool PCloudApp::new_version() //for pcloudwin
{
    return this->newVersionFlag;
}

//a slot called when user chooses Remind me later for new version from about page
void PCloudApp::setTimerInterval(int index)
{
    settings->setValue("vrsnNotifyInvervalIndx", index); //sets the current index for the combo in About page also
    int notifyTimeInterval;
    switch(index)
    {
    case 0: //1 hour
        // notifyTimeInterval = 30; //temp for qa
        notifyTimeInterval = 3600;
        qDebug()<< QDateTime::currentDateTime() <<"NOTIFICATIONS (setTimerInterval case 0 - 1h): new time for notify is set for: " << QDateTime::currentDateTime().addSecs(notifyTimeInterval);
        break;
    case 1: //6 hours
        notifyTimeInterval = 21600;
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 1 - 6h): new time for notify is set for: " << QDateTime::currentDateTime().addSecs(notifyTimeInterval);
        break;
    case 2: //24 hours
        notifyTimeInterval = 86400;
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 2 - 24h): new time for notify is set for: " << QDateTime::currentDateTime().addSecs(notifyTimeInterval);
        break;
    case 3: //after a week
        notifyTimeInterval = 604800;
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 3 - a week): new time for notify is set for: " << QDateTime::currentDateTime().addSecs(notifyTimeInterval);
        break;
    case 4:
        notifyTimeInterval = 0;
        this->stopTimer();
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 4 - Never): stop timer if has started"
               <<"isActive" << updateNtfctnTimer->isActive();
        return;
    default:
        return;
    }

    updateNtfctnTimer->setInterval(notifyTimeInterval*1000); //refresh timer when another interval is selected
    qDebug()<<"update interval case: "<<updateNtfctnTimer->interval()/1000<<updateNtfctnTimer->isSingleShot()<<updateNtfctnTimer->isActive(); //to del after qa
    if(!updateNtfctnTimer->isActive()) //came from Never case
        updateNtfctnTimer->start();

}
void PCloudApp::showPopupNewVersion()
{
    qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS show popup message";
    tray->showMessage("New Version", "A new version of pCloud Drive is available!\nClick here for more details");
}
void PCloudApp::stopTimer()
{
    if(updateNtfctnTimer->isActive())
        updateNtfctnTimer->stop();
}
/* OLD SPECIFICATION
//a slot called when user chooses Remind me later for new version from about page
void PCloudApp::setTimerInterval(int index)
{
    settings->setValue("vrsnNotifyInvervalIndx", index); //sets the current index for the combo in About page
    QDateTime now = QDateTime::currentDateTime(), NewDateTimeForNotify;
    switch(index)
    {
    case 0: //1 hour
        //NewDateTimeForNotify = now.addSecs(3600); // for tests
        NewDateTimeForNotify = now.addSecs(30);
        qDebug()<< QDateTime::currentDateTime() <<"NOTIFICATIONS (setTimerInterval case 0 - 30 secs): new time for notify is set for: " << NewDateTimeForNotify;
        break;
    case 1: //6 hours
        //NewDateTimeForNotify = now.addSecs(21600);
        NewDateTimeForNotify = now.addSecs(60);
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 1 - 60 secs): new time for notify is set for: " << NewDateTimeForNotify;
        break;
    case 2: //24 hours
        //NewDateTimeForNotify = now.addSecs(86400);
        NewDateTimeForNotify = now.addSecs(90);
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 2 - 120 secs): new time for notify is set for: " << NewDateTimeForNotify;
        break;
    case 3: //after a week
        //NewDateTimeForNotify = now.addSecs(604800);
        NewDateTimeForNotify = now.addSecs(120);
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 3 - 180 secs): new time for notify is set for: " << NewDateTimeForNotify;
        break;
    case 4:
        settings->setValue("lastAppNotifiedVrsn", APP_VERSION);
        settings->setValue("vrsnNotfyDtTime", NewDateTimeForNotify); // null
        if(versnThread != NULL) //if timer has already working for some time interval //1
        {
            if(versnThread->isRunning())
                versnThread->terminate();
            versnThread->wait();
        }
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval case 4 - Never): stop timer";
        return;
    default:
        return;
    }
    settings->setValue("vrsnNotfyDtTime", NewDateTimeForNotify);

    if(versnThread == NULL) // first choose of an interval for Remind me later or "Never" has been selected before //2
    {
        qDebug()<<QDateTime::currentDateTime() << "NOTIFICATIONS (setTimerInterval): create and start Timer";
        versnThread = new VersionTimerThread(this, NewDateTimeForNotify);
        versnThread->start();
    }
    else
    {
        qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (setTimerInterval): set new time for the Timer ";
        versnThread->setNewDateTimeForNotify(NewDateTimeForNotify); // timer is working, just reset it's new time //3
        if(!versnThread->isRunning()) // if it has been started, after that "never" selected, and now a new time interval //4
        {
            versnThread->start();
        }
    }
} */

bool PCloudApp::isMenuorWinActive()
{
    if(this->loggedmenu)
        return (this->loggedmenu->isActiveWindow() || this->pCloudWin->isVisible());
    else
        return false;
}
bool PCloudApp::isMainWinPageActive(int index)
{
    return (this->loggedin && pCloudWin->isVisible() && pCloudWin->getCurrentPage() == index);
}

// use public function to change sync icon according to statuses
//because static vars can't emit signals( signals are protected i qt4)
void PCloudApp::showLoginPublic()
{
    emit this->showLoginSgnl();
}
void PCloudApp::logoutPublic()
{
    emit this->logoutSignl();
}

void PCloudApp::updateTrayNtfIcon()
{
    this->newNtfFLag = false;
    emit changeSyncIcon(lastTrayIconIndex);
}

void PCloudApp::changeSyncIconPublic(int index)
{
    emit this->changeSyncIcon(index);
    if(index == 4 && !noFreeSpaceMsgShownFlag)
    {
        noFreeSpaceMsgShownFlag = true;
        if(this->lastStatus == PSTATUS_DISK_FULL)    // no local disk space (according to settings)
            emit this->showMsgBoxSgnl(trUtf8("pCloud Drive: System disk space low"),
                                      trUtf8("pCloud Drive might not behave properly.\nConsider freeing some space on your system drive or setting Minimum Disk Space to a lower value."), 5);
        else // pcloud account is full
            emit this->showMsgBoxSgnl(trUtf8("Account is full"),trUtf8("Your pCloud account is out of free space!"), 2); //++ get more space suggestion
    }
}
void PCloudApp::changeOnlineItemsPublic(bool logged)
{
    emit this->changeOnlineItemsSgnl(logged);
}
void PCloudApp::sendTrayMsgTypePublic(const char *title, const char *msg, int msgtype)
{
    emit this->sendTrayMsgType(title,msg,msgtype);
}

void PCloudApp::showMsgBoxPublic(QString title, QString msg, int msgIconVal)
{
    emit showMsgBoxSgnl(title,msg,msgIconVal);
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
    qDebug()<<"updateSyncStatusPublic";
    emit this->updateSyncStatusSgnl();
}

void PCloudApp::updateUserInfoPublic(const char* param)
{
    emit updateUserInfoSgnl(param);
}

void PCloudApp::addNewSharePublic(QString fldrPath)
{
    emit addNewShareSgnl(fldrPath);
}

void PCloudApp::addNewSyncPublic()
{
    emit addNewSyncSgnl();
}

void PCloudApp::addNewSyncLstPublic(bool addLocalFldrs)
{
    emit addNewSyncLstSgnl(addLocalFldrs);
}

void PCloudApp::unlockCryptoFldrPublic()
{
    emit this->unlockCryptoFldrSgnl();
}

void PCloudApp::lockCryptoFldrPublic()
{
    emit this->lockCryptoFldrSgnl();
}

void PCloudApp::updateNotfctnsPublic(int newcnt)
{    
    emit this->updateNotfctnsModelSgnl(newcnt);
    this->newNtfFLag = newcnt ? true : false;
    if (this->isLogedIn())
        changeSyncIconPublic(this->lastTrayIconIndex); //change current icon with the same with the notification red dot
}

void PCloudApp::setsyncSuggstLst(QStringList lst)
{
    this->syncSuggstLst = lst;
    qDebug()<<"PCloudApp:setsyncSuggstLst"<<this->syncSuggstLst;
}
void PCloudApp::refreshSyncUIitemsPublic()
{
    emit refreshSyncUIitemsSgnl();
}

void PCloudApp::setErrText(int win, const char *err)
{
    switch(win)
    {
    case 1:
        if (this->logwin)
            logwin->showError(err);
        break;
    default:
        break;
    }
}

void PCloudApp::setLogWinError(const char *msg)
{
    if (this->logwin)
        this->logwin->showError(msg);
}

void PCloudApp::setTrayIcon(int index)
{    
    if(this->newNtfFLag && this->isLogedIn())
        tray->setIcon(QIcon(icons[index][1]));
    else
        tray->setIcon(QIcon(icons[index][0]));

    this->lastTrayIconIndex = index;
}
void PCloudApp::showTrayMsgType(const char *title, const char *msg, int msgtype)
{
    this->lastMessageType = msgtype;
    tray->showMessage(QString::fromStdString(title),QString::fromStdString(msg));
}

void PCloudApp::showMsgBox(QString title, QString msg, int msgIconVal)
{    
    switch(msgIconVal)
    {
    case 2: //show warning msg
        QMessageBox::warning(NULL,title,msg);
        break;
    case QMessageBox::Critical:
        QMessageBox::critical(NULL,title,msg);
        break;
    case  5:  //disk full , other cases are already defined in qt
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(title);
        msgBox.setText(msg);
        msgBox.setIcon(QMessageBox::Warning);
        QPushButton *editSettingsBtn = msgBox.addButton(trUtf8(" Edit Settings "), QMessageBox::AcceptRole);
        msgBox.setDefaultButton(editSettingsBtn);
        msgBox.setStandardButtons(QMessageBox::Cancel);
        if (msgBox.exec()== QMessageBox::AcceptRole)
        {
            this->showSettings();
            int tabIndexSpace;
#ifdef Q_OS_LINUX
            tabIndexSpace = 1;
#else
            tabIndexSpace = 2;
#endif
            pCloudWin->setPageCurrentTab(SETTINGS_PAGE_NUM,tabIndexSpace);
        }
        break;
    }
    default:
        break;
    }
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
    //  pCloudWin->ui->btnPauseSync->setVisible(false);
    // pCloudWin->ui->btnResumeSync->setVisible(true);
}
void PCloudApp::resumeSync()
{
    psync_resume();
    pauseSyncAction->setVisible(true);
    resumeSyncAction->setVisible(false);
    //pCloudWin->ui->btnPauseSync->setVisible(true);
    //pCloudWin->ui->btnResumeSync->setVisible(false);
}
/*
void PCloudApp::clearSyncFolderActions()
{
    int actnsNum = this->syncMenu->actions().size();
    qDebug()<<"PCloudApp::createSyncFolderActions"<<actnsNum;
    QList<QAction*> list = syncMenu->actions();
    for (int i = 0; i < list.size(); i++)
    {
        qDebug()<<i<<list[i]->data().toString();
    }


    if (actnsNum > 3)
    {
        for (int i = 0; i < actnsNum - 3; i++)
        {
            QAction* action = syncMenu->actions()[i];
            syncMenu->removeAction(action);
            delete action;
        }
    }
}
*/

void PCloudApp::createSyncFolderActions() //refreshes menu when user rename/delete local root sync folder, add new folder through context menu..
{
    /* this->clearSyncFolderActions();

    psync_folder_list_t *fldrsList = psync_get_sync_list();
    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (uint i = 0; i < fldrsList->foldercnt; i++)
        {
            QDir localDir(fldrsList->folders[i].localpath);
            if (localDir.exists())
            {
                QAction *fldrAction = new QAction(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),fldrsList->folders[i].localname,this);
                fldrAction->setProperty("path", fldrsList->folders[i].localpath);
                connect(fldrAction, SIGNAL(triggered()),this, SLOT(openLocalDir()));
                this->syncMenu->addAction(fldrAction);
            }
        }
        free(fldrsList);
    }
*/


    this->syncedFldrsMenu->clear(); //Actions owned by the menu and not shown in any other widget are deleted by this func

    psync_folder_list_t *fldrsList = psync_get_sync_list();

    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (uint i = 0; i < fldrsList->foldercnt; i++)
        {
            QDir localDir(fldrsList->folders[i].localpath);
            if (localDir.exists())
            {
                QAction *fldrAction = new QAction(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),fldrsList->folders[i].localname,this);
                fldrAction->setProperty("path", fldrsList->folders[i].localpath);
                connect(fldrAction, SIGNAL(triggered()),this, SLOT(openLocalDir()));
                this->syncedFldrsMenu->addAction(fldrAction);
            }
        }
        free(fldrsList);
    }
}

//when user selects it from the menu
void PCloudApp::openLocalDir()
{
    QObject *menuItem = QObject::sender();
    QString path = menuItem->property("path").toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void PCloudApp::lockCryptoFldr()
{
    pCloudWin->getCryptoPage()->lock();
}

void PCloudApp::unlockCryptoFldr()
{
    pCloudWin->getCryptoPage()->unlock();
}

void PCloudApp::openCryptoFldr()
{
    pCloudWin->getCryptoPage()->openCryptoFldr();
}

void PCloudApp::addNewFolderInMenu(QAction *fldrAction) //for add new sync case
{
    this->syncedFldrsMenu->addAction(fldrAction);
    //this->createSyncFolderActions();
}

void PCloudApp::addNewShare()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!sharefolderwin)
        sharefolderwin = new ShareFolderWindow(this, pCloudWin, NULL);
    else
        sharefolderwin->setContextMenuFlag(false);
    showWindow(sharefolderwin);
    QApplication::restoreOverrideCursor();
}

void PCloudApp::addNewShare(QString fldrPath) // from context menu
{
    qDebug()<<"addNewShare path"<<fldrPath;
    this->hideAllWindows();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!sharefolderwin)
        sharefolderwin = new ShareFolderWindow(this, pCloudWin, fldrPath);
    else
    {
        sharefolderwin->setFldrbyMenu(fldrPath);
        sharefolderwin->setContextMenuFlag(true);
    }
    showWindow(sharefolderwin);
    QApplication::restoreOverrideCursor();
}

void PCloudApp::addNewSync()
{
    emit this->pCloudWin->syncPage->addSync(); // to be moved
}
void PCloudApp::addNewSyncLst(bool addLocalFldrs)
{
    hideAllWindows();
    if(syncFldrsWin == NULL)
        syncFldrsWin = new  SuggestnsBaseWin(this, addLocalFldrs, &syncSuggstLst);
    else
    {
        if(addLocalFldrs)
            syncFldrsWin->addLocalFldrs(&syncSuggstLst);
        else
            syncFldrsWin->addRemoteFldrs(&syncSuggstLst);
    }
    if(!syncFldrsWin->getItemsNum())
    {
        QMessageBox::information(NULL, "pCloud Drive", "Selected folders can not be synced. They are may be already synced or ecrypted!");
        return;
    }
    this->showWindow(syncFldrsWin);
}

//updates menu, pcloudwin and tray icon with current sync upld/downld info
void PCloudApp::updateSyncStatus()
{
    QString traymsg = this->downldInfo + "\n" + this->uploadInfo;
    syncDownldAction->setText(downldInfo);
    syncUpldAction->setText(uploadInfo);
    pCloudWin->ui->label_dwnld->setText(downldInfo);
    pCloudWin->ui->label_upld->setText(uploadInfo);

    this->tray->setToolTip(traymsg);
}

//called when add/remove sync from context menu or suggestions
void PCloudApp::refreshSyncUIitems()
{
    this->createSyncFolderActions();
    this->pCloudWin->get_sync_page()->load();
}

void PCloudApp::refreshTray()
{
    this->updateSyncStatus();
    this->setCryptoAction();
}

void PCloudApp::setCryptoAction()
{
    int cryptoInx = pCloudWin->getCryptoPage()->getCurrentCryptoPageIndex();
    if(!cryptoInx || cryptoInx == 1) // crypto hasn't setup
    {
        cryptoWelcomeAction->setVisible(true);
        cryptoFldrLockedAction->setVisible(false);
        cryptoUnlockedMenuAction->setVisible(false);
    }
    else if(psync_crypto_isstarted()) // folder is unlocked
    {
        cryptoWelcomeAction->setVisible(false);
        cryptoFldrLockedAction->setVisible(false);
        cryptoUnlockedMenuAction->setVisible(true);
    }
    else
    {
        cryptoWelcomeAction->setVisible(false);
        cryptoFldrLockedAction->setVisible(true);
        cryptoUnlockedMenuAction->setVisible(false);
    }
}

void PCloudApp::updateUserInfo(const char* &param)
{
    if (!strcmp(param, "quota"))
        this->getQuota();
    else
        this->getUserInfo();

    userinfoAction->setText(this->usedSpaceStr + " ("+
                            QString::number(100 - this->freeSpacePercentage) + "%) of " + this->planStr + " used");
    emit this->pCloudWin->refreshUserinfo();
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
        return QString::number(res, 'f', 2) + " GB";
    }
    else
    {
        qreal res = static_cast<double>(bytes) / (one<<40);
        return QString::number(res, 'f' ,2) + " TB";
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
    if (state == QNetworkSession::Roaming || state == QNetworkSession::NotAvailable || state == QNetworkSession::Connected)
        psync_network_exception();
    if((state == QNetworkSession::Connected || state == QNetworkSession::Roaming) && this->nointernetFlag)
    {
        nointernetFlag = false;
        if(isLogedIn())
        {
            changeSyncIconPublic(1);
            changeOnlineItems(true);
        }
        else
            this->showLoginPublic();
    }
    if((state == QNetworkSession::Disconnected || state == QNetworkSession::NotAvailable) && !this->nointernetFlag)
        this->nointernetFlag = true;
}

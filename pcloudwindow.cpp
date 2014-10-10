#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "changepassdialog.h"
#include "ui_changepassdialog.h"
#include "common.h"
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QDebug>


PCloudWindow::PCloudWindow(PCloudApp *a,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PCloudWindow)
{
    app=a;
    ui->setupUi(this);
    this->verifyClicked = false;
    vrsnDwnldThread = NULL;

    // the window consists of QListWidget(for icon-buttons) and
    //QStackedWidget which loads different page according to selected item in the listWidget (and hides other pages)

    ui->listButtonsWidget->setViewMode(QListView::IconMode);
    ui->listButtonsWidget->setFlow(QListWidget::LeftToRight); //orientation
    ui->listButtonsWidget->setSpacing(12);
    ui->listButtonsWidget->setStyleSheet("background-color:transparent");
    ui->listButtonsWidget->setMovement(QListView::Static); //not to move items with the mouse
    ui->listButtonsWidget->setMinimumWidth(450);
    ui->listButtonsWidget->setMaximumHeight(85);
    ui->listButtonsWidget->setMinimumHeight(84); // precakva mi layouta

    //create Items for QListWidget
    new QListWidgetItem(QIcon(":/128x128/images/128x128/user.png"),trUtf8("Account"),ui->listButtonsWidget); //index 0
    new QListWidgetItem(QIcon(":/128x128/images/128x128/user.png"),trUtf8("Account"),ui->listButtonsWidget); //index 1
    new QListWidgetItem(QIcon(":/128x128/images/128x128/drive.png"),trUtf8("Drive"),ui->listButtonsWidget); //index 2
    new QListWidgetItem(QIcon(":/128x128/images/128x128/sync.png"),trUtf8("Sync"),ui->listButtonsWidget); //Sync Page index 3
    new QListWidgetItem(QIcon(":/128x128/images/128x128/shares.png"),trUtf8("Shares"),ui->listButtonsWidget); //index 4
    new QListWidgetItem(QIcon(":/128x128/images/128x128/settings.png"),trUtf8("Settings"),ui->listButtonsWidget); //index 5
    new QListWidgetItem(QIcon(":/128x128/images/128x128//help.png"),trUtf8("Help"),ui->listButtonsWidget); //index 6
    new QListWidgetItem(QIcon(":/128x128/images/128x128/info.png"),trUtf8("About"),ui->listButtonsWidget); //index 7

    fillAcountNotLoggedPage();
    fillAboutPage();
    fillDrivePage();
    settngsPage = new SettingsPage(this, app);
    syncPage = new SyncPage(this, app);
    sharesPage = new SharesPage(this, app);
    // indexes of Items in listWidget and their coresponding pages in StackWidget are the same
    connect(ui->listButtonsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->btnVerify, SIGNAL(clicked()), this, SLOT(verifyEmail()));
    connect(ui->btnShareFolder, SIGNAL(clicked()), app, SLOT(addNewShare()));
    connect(this, SIGNAL(refreshPageSgnl(int,int)), this, SLOT(refreshPageSlot(int,int)));

    //for resize
    for(int i = 0; i < ui->pagesWidget->count(); i++)
        ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Sync");
    // ui notes - set statusbar max size to (0,0)

    connect(ui->btnLogin, SIGNAL(clicked()),app, SLOT(showLogin()));
    connect(ui->btnRegstr, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->btnExit, SIGNAL(clicked()), app, SLOT(doExit()));
    connect(ui->btnDriveOpenFldr, SIGNAL(clicked()), app, SLOT(openCloudDir()));
    connect(ui->btnDriveStart, SIGNAL(clicked()), this, SLOT(launchFS()));
    connect(ui->btnDriveStop, SIGNAL(clicked()), this, SLOT(launchFS()));
    connect(ui->toolBtnContact, SIGNAL(clicked()), this, SLOT(contactUs()));
    connect(ui->tBtnOnlineHelp, SIGNAL(clicked()), this, SLOT(openOnlineHelp()));
    connect(ui->tBtnOnlineTutorial, SIGNAL(clicked()), this, SLOT(openOnlineTutorial()));
    connect(ui->tBtnFeedback, SIGNAL(clicked()), this, SLOT(sendFeedback()));
    connect(ui->tbtnGetMoreSpace, SIGNAL(clicked()), this, SLOT(upgradePlan()));
    connect(ui->tbtnMyPcloud, SIGNAL(clicked()), this, SLOT(openMyPcloud()));
    connect(ui->comboBox_versionReminder, SIGNAL(currentIndexChanged(int)), app, SLOT(setTimerInterval(int)));
    connect(ui->btnUpdtVersn, SIGNAL(clicked()), this, SLOT(updateVersion()));
    //p connect(ui->tbtnOpenFolder, SIGNAL(clicked()),app,SLOT(openCloudDir()));
    connect(ui->btnLgout, SIGNAL(clicked()), app, SLOT(logOut()));
    connect(ui->btnUnlink, SIGNAL(clicked()), this, SLOT(unlinkSync()));

    QMenu *menuAccnt = new QMenu(this);
    QAction *actionChangePass = new QAction(trUtf8("Change Password"),this);
    connect(actionChangePass, SIGNAL(triggered()), this, SLOT(changePass()));
    QAction *forgotPassAction = new QAction(trUtf8("Forgot Password"), this);
    connect(forgotPassAction, SIGNAL(triggered()), this, SLOT(forgotPass()));
    menuAccnt->addAction(actionChangePass);
    menuAccnt->addAction(forgotPassAction);
    ui->btnAccntMenu->setMenu(menuAccnt);
    this->setMinimumHeight(560);
    //this->setMinimumWidth(1024);

}

PCloudWindow::~PCloudWindow()
{    
    if(vrsnDwnldThread)
    {
        if(vrsnDwnldThread->isRunning())
            vrsnDwnldThread->terminate();
        vrsnDwnldThread->wait();
        delete vrsnDwnldThread;
    }
    delete ui;
}

void PCloudWindow::closeEvent(QCloseEvent *event) //not to close the app after window close
{
    hide();
    event->ignore();
}

void PCloudWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    int currentIndex = ui->listButtonsWidget->row(current);

    // auto resize
    for(int i = 0; i < ui->pagesWidget->count(); i++)
    {
        if ( i != currentIndex)
            // ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    }
    //ui->pagesWidget->widget(currentIndex)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->pagesWidget->widget(currentIndex)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    //setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    updateGeometry();
    ui->pagesWidget->setCurrentIndex(currentIndex); // sets page
    refreshPage(currentIndex);

}
void PCloudWindow::showEvent(QShowEvent *)
{    
    refreshPage(ui->listButtonsWidget->currentRow());
}

void PCloudWindow::refreshPage(int currentIndex)
{            
    switch(currentIndex)
    {
    case ACCNT_LOGGED_PAGE_NUM:
        if(verifyClicked)                 // Account page, case when user just has clicked Verify Email
            checkVerify();
        break;
    case SHARES_PAGE_NUM:
        sharesPage->loadAll();
        break;
    case SETTINGS_PAGE_NUM:
        settngsPage->showEvent();
        break;
    case ABOUT_PAGE_NUM:                    //About page, when have a new version
        if(app->new_version())
            fillAboutPage();
        break;
    default:
        break;
    }
}

void PCloudWindow::setCurrntIndxPclWin(int index)
{
    ui->listButtonsWidget->setCurrentRow(index);
    ui->pagesWidget->setCurrentIndex(index);

}

void PCloudWindow::setOnlineItems(bool online) // change pcloud window menu when is loggedin and loggedout
{
    if(online)
    {
        ui->listButtonsWidget->setRowHidden(ACCNT_NOT_LOGGED_PAGE_NUM, true); //Accont - not logged
        ui->listButtonsWidget->setRowHidden(ACCNT_LOGGED_PAGE_NUM, false); //Account - logged
#ifdef VFS
        ui->listButtonsWidget->setRowHidden(DRIVE_PAGE_NUM, false); //drive
#endif
        ui->listButtonsWidget->setRowHidden(SYNC_PAGE_NUM, false); //Sync
        ui->listButtonsWidget->setRowHidden(SHARES_PAGE_NUM, false); //Shares
        ui->listButtonsWidget->setRowHidden(SETTINGS_PAGE_NUM, false);       //setttings
    }
    else
    {
        ui->listButtonsWidget->setRowHidden(ACCNT_NOT_LOGGED_PAGE_NUM, true);
        ui->listButtonsWidget->setRowHidden(ACCNT_LOGGED_PAGE_NUM, true);
        ui->listButtonsWidget->setRowHidden(DRIVE_PAGE_NUM, true);
        ui->listButtonsWidget->setRowHidden(SYNC_PAGE_NUM,true);
        ui->listButtonsWidget->setRowHidden(SHARES_PAGE_NUM,true);
        ui->listButtonsWidget->setRowHidden(SETTINGS_PAGE_NUM, true);
    }
}

void PCloudWindow::setOnlinePages()
{    
    fillAccountLoggedPage();
}
void PCloudWindow::fillAcountNotLoggedPage()
{
    ui->toolBtnContact->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
}

void PCloudWindow::fillDrivePage()
{
    bool isFSStarted = psync_fs_isstarted();
    qDebug()<< "fill drive page, is fs started:" << isFSStarted << psync_fs_getmountpoint();
    if(isFSStarted)
    {
        ui->btnDriveStart->setVisible(false);
        ui->btnDriveStop->setVisible(true);
        ui->btnDriveOpenFldr->setVisible(true);
    }
    else
    {
        ui->btnDriveStart->setVisible(true);
        ui->btnDriveOpenFldr->setVisible(false);
        ui->btnDriveStop->setVisible(false);
    }
}

void PCloudWindow::fillAboutPage()
{
    if(!app->new_version())
    {
        ui->label_versionVal->setText(QString("Version ") + APP_VERSION + QString("\n\nEverything is up to date"));
        ui->label_versionVal->setAlignment(Qt::AlignHCenter);
        ui->widget_newVersion->setVisible(false);
    }
    else
    {
        ui->label_versionVal->setText(QString ("Version ") + APP_VERSION);
        ui->label_newVersion->setText(QString("New version "  + app->newVersion.versionstr + " has already been released"));
        ui->label_notes->setText(QString("Notes:\n "+ app->newVersion.notes));
        ui->comboBox_versionReminder->setCurrentIndex(app->settings->value("vrsnNotifyInvervalIndx").toInt());
    }
}

void PCloudWindow::fillAccountLoggedPage()
{        
    ui->label_email->setText(app->username);
    if (app->isVerified)
    {
        ui->btnVerify->setVisible(false);
        ui->checkBoxVerified->setVisible(true);
        ui->checkBoxVerified->setCheckState(Qt::Checked);
        ui->checkBoxVerified->setEnabled(false);
    }
    else
    {
        ui->checkBoxVerified->setVisible(false);
        ui->btnVerify->setVisible(true);
    }
    ui->label_space->setText(QString::number(app->usedSpace, 'f', 2) + "GB (" + QString::number(app->freeSpacePercentage) + "% free)");
    ui->label_planVal->setText(app->planStr);

    if (app->isPremium)
    {
        quint64 unixTime = psync_get_uint_value("premiumexpires");
        QDateTime timestamp;
        timestamp.setTime_t(unixTime);
        ui->label_expDtVal->setText(timestamp.toString("dd.MM.yyyy"));
    }
    else
    {
        ui->label_expDt->setVisible(false);
        ui->label_expDtVal->setVisible(false);
    }

    //for sync hide some widgets till start use new fs
    ui->tbtnOpenFolder->setVisible(false);

}
void PCloudWindow::refreshUserinfo()
{
    this->fillAccountLoggedPage();
}

static QList<QTreeWidgetItem *> listRemoteFldrs(QString parentPath)
{
    QList<QTreeWidgetItem *> items;
    pfolder_list_t *res = psync_list_remote_folder_by_path(parentPath.toUtf8(),PLIST_FOLDERS);

    if (res != NULL)
    {
        for(uint i = 0; i < res->entrycnt; i++)
        {
            QString path = parentPath;
            if (parentPath != "/")
                path.append("/").append(res->entries[i].name);
            else
                path.append(res->entries[i].name);

            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(res->entries[i].name));
            item->setIcon(0, QIcon(":images/images/folder-p.png"));
            item->setData(0, Qt::UserRole, path);
            item->setData(1, Qt::UserRole, (quint64)res->entries[i].folder.folderid);
            item->addChildren(listRemoteFldrs(path));
            items.append(item);
        }
    }

    free(res);
    return items;
}

void PCloudWindow::initRemoteTree(QTreeWidget *table)
{
    //pcloud folders
    QList<QTreeWidgetItem *> items;
    table->clear();
    table->setColumnCount(1);
    table->setHeaderLabels(QStringList("Name"));
    QString root = "/";
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(QStringList(root));
    rootItem->setIcon(0,QIcon(":images/images/folder-p.png"));
    rootItem->setData(0, Qt::UserRole,root); //set path
    rootItem->setData(1,Qt::UserRole,0); //id
    table->insertTopLevelItem(0,rootItem);
    table->setCurrentItem(rootItem);
    items = listRemoteFldrs(root);
    rootItem->addChildren(items);
    table->expandItem(rootItem);
    table->setSortingEnabled(true);
    table->sortByColumn(0, Qt::AscendingOrder);
}

QString PCloudWindow::newRemoteFldr(QTreeWidget *table)
{
    QString dirname = QInputDialog::getText(this,
                                            tr("Create Directory"),
                                            tr("Directory name"));
    if(dirname.isEmpty())
        return "";
    QString parentpath = table->currentItem()->data(0, Qt::UserRole).toString();
    if(parentpath != "/")
        parentpath.append("/");
    parentpath.append(dirname);

    char *err = NULL;
    psync_create_remote_folder_by_path(parentpath.toUtf8(),&err);
    if (err)
    {
        QMessageBox::critical(this,"pCloud",trUtf8(err));
        return "";
    }
    free(err);

    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0,QStringList(dirname));
    item->setIcon(0,QIcon(":images/images/folder-p.png"));
    item->setData(0,Qt::UserRole, parentpath);
    table->currentItem()->insertChild(0,item);
    table->setCurrentItem(item);
    table->scrollToItem(item);

    return dirname;
}

int PCloudWindow::getCurrentPage()
{
    return this->ui->listButtonsWidget->currentRow();
}

SyncPage* PCloudWindow::get_sync_page()
{
    return this->syncPage;
}

void PCloudWindow::refreshPagePulbic(int pageindex, int param)
{
    emit this->refreshPageSgnl(pageindex, param);
}

//SLOTS
void PCloudWindow::launchFS()
{
    if(QObject::sender()->objectName() == "btnDriveStart")
    {
        qDebug()<<"start fs";
        psync_fs_start();
        ui->btnDriveStart->setVisible(false);
        ui->btnDriveOpenFldr->setVisible(true);
        ui->btnDriveStop->setVisible(true);
    }
    else
    {
        qDebug()<<"stop fs";
        psync_fs_stop();
        ui->btnDriveStart->setVisible(true);
        ui->btnDriveOpenFldr->setVisible(false);
        ui->btnDriveStop->setVisible(false);
    }
}

void PCloudWindow::changePass()
{
    ChangePassDialog *dialog = new ChangePassDialog();
    dialog->ui->label_email->setText(app->username);
    if (dialog->exec() == QDialog::Accepted)
    {
        char *err = NULL;
        int res =  psync_change_password(dialog->ui->line_pass->text().toUtf8(), dialog->ui->line_newpass->text().toUtf8(), &err);
        if (!res)
        {
            psync_set_pass(dialog->ui->line_newpass->text().toUtf8(),0);
            QMessageBox::information(this,trUtf8("New password"), trUtf8("Password successfully changed!"));
        }
        else
        {
            if (res == -1 )
                QMessageBox::information(this,trUtf8("New password"), trUtf8("No internet connection"));
            else
                QMessageBox::information(this,trUtf8("New password"), trUtf8(err));
        }
        free(err);
    }

}
void PCloudWindow::forgotPass()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    char *err = NULL;
    int res = psync_lost_password(app->username.toUtf8(),&err);
    if (!res) // returns 0 on success
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, trUtf8("Reset password"), trUtf8("An email with password reset instructions is sent to your address."));
    }
    else
    {
        if (res == -1)
        {
            QMessageBox::information(this, trUtf8("Reset password"), trUtf8("No internet connection"));
            QApplication::restoreOverrideCursor();
            return;
        }
        else
        {
            QMessageBox::information(this, trUtf8("Reset password"), trUtf8(err));
            QApplication::restoreOverrideCursor();
            return;
        }
    }
    QApplication::restoreOverrideCursor();
    free(err);
}

void PCloudWindow::unlinkSync()
{   
    QMessageBox::StandardButton reply;
    //reply = QMessageBox::warning(this,trUtf8("Unlink"), trUtf8("If you unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
    reply = QMessageBox::warning(this,trUtf8("Unlink"),
                                 trUtf8("By unlinking you will lose all pCloud Drive settings for the current account on this computer. Are you sure?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
        app->unlink();
}

void PCloudWindow::verifyEmail(){    // not implemented
    char *err = NULL;
    int res = psync_verify_email(&err);
    if (!res)
    {
        QMessageBox::information(this, "Verify e-mail", "E-mail verification sent to: "+app->username);
        verifyClicked = true;
    }
    else
    {
        if(res == -1)
            QMessageBox::information(this,trUtf8("Verify e-mail"), trUtf8("No internet connection"));
        else
            QMessageBox::information(this,trUtf8("Verify e-mail"), trUtf8(err));
    }
    free(err);
}

void PCloudWindow::checkVerify() // has the user verified after had clicked "Verify Now"
{
    bool verified = psync_get_bool_value("emailverified");
    if (verified)
    {
        ui->checkBoxVerified->setVisible(true);
        ui->checkBoxVerified->setChecked(true);
        ui->checkBoxVerified->setEnabled(false);
        ui->btnVerify->setVisible(false);
        verifyClicked = false;
    }
}
void PCloudWindow::updateVersion()
{        
    app->stopTimer();
    QMessageBox::information(this,"pCloud Sync",trUtf8( "The new version of pCloud starts downloading and prepearing to install.\n Please wait."));
    if(!vrsnDwnldThread)
        vrsnDwnldThread = new VersionDwnldThread(app->OSStr);
    vrsnDwnldThread->start();
}

void PCloudWindow::refreshPageSlot(int pageindex, int param)
{    
    switch(pageindex)
    {
    case DRIVE_PAGE_NUM:
        this->fillDrivePage();
    case SHARES_PAGE_NUM:  // sharespage
        this->sharesPage->refreshTab(param);
    }
}

void PCloudWindow::openMyPcloud()
{
    QUrl url("https://my.pcloud.com/#page=filemanager&authtoken="+app->authentication);
    QDesktopServices::openUrl(url);
}

void PCloudWindow::upgradePlan()
{
    QUrl url("https://my.pcloud.com/#page=plans&authtoken="+app->authentication);
    QDesktopServices::openUrl(url);
}

void PCloudWindow::sendFeedback()
{
    QUrl url("https://my.pcloud.com/#page=contact"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::openOnlineTutorial(){

    QUrl url ("https://my.pcloud.com/#page=contact&tutorial=show"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::openOnlineHelp()
{
    //QUrl url ("https://my.pcloud.com/#page=faq"); //to check
    QUrl url("https://www.pcloud.com/support.html#p=8");
    QDesktopServices::openUrl(url);
}

void PCloudWindow::contactUs(){
    QUrl url ("https://my.pcloud.com/#page=contact");
    QDesktopServices::openUrl(url);
}

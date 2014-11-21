#include "syncpage.h"
#include "psynclib.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"
#include "modifysyncdialog.h"
#include "ui_modifysyncdialog.h"
#include "common.h"
#include <QRegExp>
#include<QDesktopServices>
#include <QUrl>
#include <QDir>
#include <unistd.h>
#include <QDebug> //temp


SyncPage::SyncPage(PCloudWindow *w, PCloudApp *a, QWidget *parent) :
    QWidget(parent)
{
    win = w;
    app = a;

    initSyncPage();

    //word wrap property breaks the layouts min size policy
    win->ui->label_syncinfo->setText("With Sync you can synchronize\n"
                                     "the content in multiple folders from your\n"
                                     "computer in real time.By synchronizing\n"
                                     "folders in pCloud Drive you make them\navailable even in offline mode.");
 #ifndef Q_OS_WIN
    win->ui->label_syncinfo->setFont(app->smaller1pFont);
    win->ui->label_infoexptns->setFont(app->smaller2pFont);
#endif

    win->ui->label_infoexptns->setText("The patterns you enter (folder name, file name, extension, etc) will be ignored while synchronizing data\n"
                                       "with your pCloud Account.Allowed wildcards: asterisk(*), question mark(?), patterns separated by semicolon(;).");

    win->ui->label_dwnld->setFont(app->smaller1pFont);
    win->ui->label_dwnld->setEnabled(false);
    win->ui->label_upld->setFont(app->smaller1pFont);
    win->ui->label_upld->setEnabled(false);
    win->ui->tabWidgetSync->setTabText(0, trUtf8("Synced Folders"));
    win->ui->tabWidgetSync->setTabText(1, trUtf8("Advanced Settings"));
    win->ui->tabWidgetSync->setCurrentIndex(0);
    win->ui->treeSyncList->setSelectionBehavior(QAbstractItemView::SelectRows);
    win->ui->treeSyncList->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList headerTitles;
    headerTitles << "Local Folder"<<""<<"pCloud Drive Folder"<<"";
    win->ui->treeSyncList->setHeaderLabels(headerTitles);
    win->ui->treeSyncList->setMinimumWidth(450);
#if QT_VERSION>QT_VERSION_CHECK(5,0,0)
    win->ui->treeSyncList->header()->setSectionsMovable(false);
#else
    win->ui->treeSyncList->header()->setMovable(false);
#endif
    // win->ui->treeSyncList->setSortingEnabled(true);
    //win->ui->treeSyncList->sortByColumn(0, Qt::AscendingOrder);
    int tableWidth =  win->ui->treeSyncList->width() - 64;
    win->ui->treeSyncList->setColumnWidth(0, (int)tableWidth/2);
    win->ui->treeSyncList->setColumnWidth(2, (int)tableWidth/2);
    win->ui->treeSyncList->setColumnWidth(1, 32);
    win->ui->treeSyncList->setColumnWidth(3, 32);
    //win->ui->label_syncinfo->setFont(app->smaller1pFont);
    win->ui->label_dwnld->setText(app->downldInfo);
    win->ui->label_upld->setText(app->uplodInfo);

    connect(win->ui->tabWidgetSync, SIGNAL(currentChanged(int)), this, SLOT(refreshTab(int)));
    connect(win->ui->treeSyncList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(stopSync(QTreeWidgetItem*,int)));
    connect(win->ui->treeSyncList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(syncDoubleClicked(QTreeWidgetItem*,int)));
    connect(win->ui->btnSyncAdd, SIGNAL(clicked()), this, SLOT(addSync()));
    //connect(win->ui->btnSyncModify,SIGNAL(clicked()), this, SLOT(modifySync()));
    win->ui->btnSyncModify->setVisible(false); // to del
    win->ui->btnSyncStop->setVisible(false); // to del
    //connect(win->ui->btnSyncStop, SIGNAL(clicked()), this, SLOT(stopSync()));
    // connect(win->ui->btnResumeSync, SIGNAL(clicked()), app, SLOT(resumeSync()));
    // connect(win->ui->btnPauseSync, SIGNAL(clicked()), app, SLOT(pauseSync()));
    connect(win->ui->btnSyncSttngsSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnSyncSttngCancel, SIGNAL(clicked()), this, SLOT(cancelSettings()));
    connect(win->ui->text_patterns, SIGNAL(textChanged()), this, SLOT(enableSaveBtn()));   
}

void SyncPage::openTab(int index)
{
    win->ui->tabWidgetSync->setCurrentIndex(index);
}

void SyncPage::refreshTab(int index)
{
    if (index)
    {
        qDebug() << "load sync settngs";
        loadSettings();
    }
    else
    {
        qDebug()<< "load sync tab";
        QApplication::setOverrideCursor(Qt::WaitCursor);
        load();
        QApplication::restoreOverrideCursor();
    }
}
static QString get_sync_type(int synctype)
{
    switch(synctype)
    {
    case PSYNC_DOWNLOAD_ONLY:
        return "Download only";
    case PSYNC_UPLOAD_ONLY:
        return "Upload only";
    case PSYNC_FULL:
        return "Download and Upload";
    default:
        return "";
        break;
    }
}

void SyncPage::load()
{
    if(win->ui->treeSyncList->topLevelItemCount())
        win->ui->treeSyncList->clear();
    //there is a delay with the result from lib after add_sync_delay
    QApplication::setOverrideCursor(Qt::WaitCursor);
    sleep(2);
    psync_folder_list_t *fldrsList = psync_get_sync_list();
    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (uint i = 0; i< fldrsList->foldercnt; i++)
        {
            qDebug()<<"Load sync tab"<< fldrsList->folders[i].folderid << " "<<fldrsList->folders[i].localname<< " "
                   <<fldrsList->folders[i].localpath << " "<<fldrsList->folders[i].remotename
                  << " "<<fldrsList->folders[i].remotepath<< " sync id="<<fldrsList->folders[i].syncid<< "sync type "<<fldrsList->folders[i].synctype;

            QStringList row;
            row <<fldrsList->folders[i].localpath <<""<< fldrsList->folders[i].remotepath;
            QTreeWidgetItem *item = new QTreeWidgetItem(row);
            item->setIcon(0,QIcon(":/32x32/images/32x32/empty-folder.png"));
            item->setIcon(1,QIcon(":/32x32/images/32x32/sync.png"));
            item->setIcon(2,QIcon(":images/images/folder-p.png"));
            item->setIcon(3,QIcon(":/32x32/images/32x32/stop-sync.png"));

            QDir localDir(fldrsList->folders[i].localpath);
            if(localDir.exists())
            {
                item->setToolTip(0,fldrsList->folders[i].localpath);
                item->setData(0, Qt::UserRole,fldrsList->folders[i].localpath);
            }
            else
            {
                item->setFlags(Qt::NoItemFlags);
                item->setToolTip(0,"Local path has changed.");
            }

            item->setData(1, Qt::UserRole, fldrsList->folders[i].synctype);
            item->setToolTip(1,get_sync_type(fldrsList->folders[i].synctype));
            item->setData(2, Qt::UserRole, fldrsList->folders[i].remotepath);
            item->setToolTip(2,fldrsList->folders[i].remotepath);
            item->setData(3, Qt::UserRole, fldrsList->folders[i].syncid);
            quint64 fldrid = fldrsList->folders[i].folderid;
            item->setData(4,Qt::UserRole, fldrid);
            win->ui->treeSyncList->insertTopLevelItem(i, item);
        }
        free(fldrsList);
    }
    QApplication::restoreOverrideCursor();
}


void SyncPage::initSyncPage()
{
    load();
    loadSettings();
}
void SyncPage::syncDoubleClicked(QTreeWidgetItem *item, int col)
{
    switch (col) {
    case 0:   //opens local folder
    {
        QString localpath = item->data(0,Qt::UserRole).toString();
        if (!localpath.isEmpty())
            QDesktopServices::openUrl(QUrl::fromLocalFile(localpath));
        else
            QMessageBox::critical(this,trUtf8("Invalid local path"), trUtf8("Local synced folder not found! It could be renamed or deleted."));
        break;
    }
    case 1: //opens modify sync type dialog
    {
        // this->modifySync();
        break;
    }
    case 2: //opens remote folder
    {
        if(psync_fs_isstarted()) //open fodler in pDrive
        {
            QString remotepath = item->data(2,Qt::UserRole).toString();
            remotepath.insert(0, QString(psync_fs_getmountpoint()).append(OSPathSeparator));
            QDesktopServices::openUrl(QUrl::fromLocalFile(remotepath));
        }
        else //open folder in web
        {
            quint64 fldrid = item->data(4,Qt::UserRole).toLongLong();
            QString urlstr = "https://my.pcloud.com/#folder=";
            urlstr.append(QString::number(fldrid) + "&page=filemanager&authtoken=" + app->authentication);
            QUrl url(urlstr);
            QDesktopServices::openUrl(url);
        }
        break;
    }
    default:
        break;
    }
}

/* obsolete
void SyncPage::modifySync() // to del
{
    QTreeWidgetItem *current = win->ui->treeSyncList->currentItem();
    if (!current)
    {
        QMessageBox::warning(this,"pCloud", trUtf8("Please select a sync!"));
        return;
    }
    else
    {
        //ModifySyncDialog modifyDialog(current->data(0, Qt::UserRole).toString(), current->data(2, Qt::UserRole).toString(), current->data(1, Qt::UserRole).toString());
        ModifySyncDialog modifyDialog(current->data(0, Qt::UserRole).toString(), current->data(2, Qt::UserRole).toString(),current->data(1, Qt::UserRole).toInt());

        if (modifyDialog.exec() == QDialog::Accepted)
        {
            //if ((psync_change_synctype(current->data(3, Qt::UserRole),(modidyDialog->ui->combo_Directions->currentIndex()+1))));
            quint32 type = modifyDialog.ui->combo_Directions->currentIndex()+1;
            quint32 syncid = current->data(3, Qt::UserRole).toInt();
            int res = psync_change_synctype(syncid,type);
            if(res  == -1)
            {
                qDebug()<<"change sync type error:";
                app->check_error();
            }
            modifyDialog.hide();
            load();
        }
    }
}
*/

void SyncPage::stopSync(QTreeWidgetItem *item, int col)
{
    if(col != 3)
        return;

    QTreeWidgetItem *current = item;
    if (!current)
    {
        QMessageBox::warning(this,"pCloud", trUtf8("Please select a sync!"));
        return;
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Stop Sync", trUtf8("Do you really want to stop selected sync?"),
                                     QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            psync_syncid_t syncid = current->data(3,Qt::UserRole).toInt();
            qDebug()<<"Stop sync with id " <<syncid;
            if (psync_delete_sync(syncid)) //0 -ok, -1 err
            {
                qDebug()<<"delete sync type error:";
                app->check_error();
            }
            else
            {
                load();
                app->createSyncFolderActions();
            }
        }
        else
            return;
    }
}

void SyncPage::addSync()
{
    addSyncDialog *addSync = new addSyncDialog(app,win, NULL);
    addSync->exec();
    delete addSync;
}

// Settings tab
void SyncPage::loadSettings()
{   
    patterns = psync_get_string_setting("ignorepatterns");
    win->ui->text_patterns->setText(patterns);

    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}

void SyncPage::enableSaveBtn()
{    
    if(win->ui->text_patterns->document()->isModified())
    {
        win->ui->btnSyncSttngsSave->setEnabled(true);
        win->ui->btnSyncSttngCancel->setEnabled(true);
    }

    else
    {
        win->ui->btnSyncSttngsSave->setEnabled(false);
        win->ui->btnSyncSttngCancel->setEnabled(false);
    }
}

void SyncPage::saveSettings()
{

    if(patterns != win->ui->text_patterns->toPlainText())
    {
        patterns = win->ui->text_patterns->toPlainText();
        psync_set_string_setting("ignorepatterns",patterns.toUtf8());
    }

    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);
}
void SyncPage::cancelSettings()
{
    loadSettings();
    win->ui->btnCancelSttngs->setEnabled(false);
    win->ui->btnSaveSttngs->setEnabled(false);
}

#include "syncpage.h"
#include "psynclib.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"
#include "modifysyncdialog.h"
#include "ui_modifysyncdialog.h"
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
    win->ui->tabWidgetSync->setTabText(0, trUtf8("Synced Folders"));
    win->ui->tabWidgetSync->setTabText(1, trUtf8("Sync Settings"));
    win->ui->tabWidgetSync->setCurrentIndex(0);

    QRegExp regExp("[1-9][0-9]{0,4}");
    win->ui->edit_minLocalSpace->setValidator(new QRegExpValidator(regExp, this));
    QRegExp regExpSpeed("[1-9][0-9]{0,4}");
    win->ui->edit_DwnldSpeed->setValidator(new QRegExpValidator(regExpSpeed, this));
    win->ui->edit_UpldSpeed->setValidator(new QRegExpValidator(regExpSpeed, this));
    win->ui->label_dwnld->setText(app->downldInfo);
    win->ui->label_upld->setText(app->uplodInfo);

    connect(win->ui->tabWidgetSync, SIGNAL(currentChanged(int)), this, SLOT(refreshTab(int)));
    connect(win->ui->treeSyncList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(syncDoubleClicked(QTreeWidgetItem*,int)));
    connect(win->ui->btnSyncModify,SIGNAL(clicked()), this, SLOT(modifySync()));
    connect(win->ui->btnSyncStop, SIGNAL(clicked()), this, SLOT(stopSync()));
    connect(win->ui->btnSyncAdd, SIGNAL(clicked()), this, SLOT(addSync()));
    connect(win->ui->btnResumeSync, SIGNAL(clicked()), app, SLOT(resumeSync()));
    connect(win->ui->btnPauseSync, SIGNAL(clicked()), app, SLOT(pauseSync()));

    connect(win->ui->btnSyncSttngsSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnSyncSttngCancel, SIGNAL(clicked()), this, SLOT(cancelSettings()));
    connect(win->ui->edit_minLocalSpace, SIGNAL(textEdited(QString)), this, SLOT(enableSaveBtn()));
    connect(win->ui->checkBoxSyncSSL, SIGNAL(stateChanged(int)), this, SLOT(enableSaveBtn()));
    connect(win->ui->checkBoxp2p, SIGNAL(stateChanged(int)), this, SLOT(enableSaveBtn()));
    connect(win->ui->text_patterns, SIGNAL(textChanged()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rBtnSyncDwldAuto, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncDwldUnlimit, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rbtnSyncDwnlChoose, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncUpldAuto, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->rBtnSyncUpldUnlimit, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->rbtnSyncupldChoose, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->edit_DwnldSpeed, SIGNAL(textEdited(QString)), this, SLOT(setNewSpeedFromEditline())); //textedited signal will not emit when the text is changed programmatically
    connect(win->ui->edit_UpldSpeed, SIGNAL(textEdited(QString)), this, SLOT(setNewSpeedFromEditline()));

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
            row << fldrsList->folders[i].localpath <<get_sync_type(fldrsList->folders[i].synctype) << fldrsList->folders[i].remotepath;
            QTreeWidgetItem *item = new QTreeWidgetItem(row);
            item->setData(0, Qt::UserRole,fldrsList->folders[i].localpath);
            QDir localDir(fldrsList->folders[i].localpath);
            if(localDir.exists())
                item->setToolTip(0,fldrsList->folders[i].localpath);
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
        QDesktopServices::openUrl(QUrl::fromLocalFile(localpath));
        break;
    }
    case 1: //opens modify sync type dialog
    {
        this->modifySync();
        break;
    }
    case 2: //opens remote folder
    {
        quint64 fldrid = item->data(4,Qt::UserRole).toLongLong();
        QString urlstr = "https://my.pcloud.com/#folder=";
        urlstr.append(QString::number(fldrid) + "&page=filemanager&authtoken=" + app->authentication);
        QUrl url(urlstr);
        QDesktopServices::openUrl(url);
        break;
    }
    default:
        break;
    }

}

void SyncPage::modifySync()
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
void SyncPage::stopSync()
{
    QTreeWidgetItem *current = win->ui->treeSyncList->currentItem();
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
    addSyncDialog *addSync = new addSyncDialog(app,win,this, NULL);
    addSync->exec();
    delete addSync;
}

// Settings tab
void SyncPage::loadSettings()
{
    SSL = psync_get_bool_setting("usessl");
    win->ui->checkBoxSyncSSL->setChecked(SSL);

    p2p = psync_get_bool_setting("p2psync");
    win->ui->checkBoxp2p->setChecked(p2p);

    minLocalSpace = QString::number((psync_get_uint_setting("minlocalfreespace"))/1024/1024);
    win->ui->edit_minLocalSpace->setText(minLocalSpace);

    //maximum upload speed in bytes per second, 0 for auto-shaper, -1 for no limit
    //donwload default - unlimitted -1
    //upload default - auto-shater 0
    dwnldSpeed = psync_get_int_setting("maxdownloadspeed");
    dwnldSpeedNew = dwnldSpeed;
    if (!dwnldSpeed)
    {
        win->ui->rBtnSyncDwldAuto->setChecked(true);
        win->ui->edit_DwnldSpeed->setEnabled(false);
    }
    else
    {
        if (dwnldSpeed == -1)
        {
            win->ui->rBtnSyncDwldUnlimit->setChecked(true);
            win->ui->edit_DwnldSpeed->setEnabled(false);
        }
        else
        {
            win->ui->rbtnSyncDwnlChoose->setChecked(true);
            win->ui->edit_DwnldSpeed->setText(QString::number(dwnldSpeed/1000));
            win->ui->rbtnSyncDwnlChoose->setEnabled(true);
        }
    }

    upldSpeed = psync_get_int_setting("maxuploadspeed");
    upldSpeedNew = upldSpeed;
    if (!upldSpeed)
    {
        win->ui->rBtnSyncUpldAuto->setChecked(true);
        win->ui->edit_UpldSpeed->setEnabled(false);
    }
    else
    {
        if (upldSpeed == -1)
        {
            win->ui->rBtnSyncUpldUnlimit->setChecked(true);
            win->ui->edit_UpldSpeed->setEnabled(false);
        }
        else
        {
            win->ui->rbtnSyncupldChoose->setChecked(true);
            win->ui->edit_UpldSpeed->setText(QString::number(upldSpeed/1000));
            win->ui->edit_UpldSpeed->setEnabled(true);
        }
    }

    patterns = psync_get_string_setting("ignorepatterns");
    win->ui->text_patterns->setText(patterns);

    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}
void SyncPage::setNewDwnldSpeed()
{    
    QObject *obj = this->sender();
    QString objname = obj->objectName();
    if (objname == "rBtnSyncDwldAuto")
    {
        dwnldSpeedNew = 0;
        win->ui->edit_DwnldSpeed->setEnabled(false);
        emit enableSaveBtn();
        return;
    }
    if (objname == "rBtnSyncDwldUnlimit")
    {
        dwnldSpeedNew = -1;
        win->ui->edit_DwnldSpeed->setEnabled(false);
        emit enableSaveBtn();
        return;
    }
    if (objname == "rbtnSyncDwnlChoose")
    {
        win->ui->edit_DwnldSpeed->setEnabled(true);

        //already entered new value but changed after that without save
        if(!win->ui->edit_DwnldSpeed->text().isEmpty())
        {
            if(dwnldSpeedNew != win->ui->edit_DwnldSpeed->text().toInt()*1000)
                dwnldSpeedNew = win->ui->edit_DwnldSpeed->text().toInt()*1000;
        }
        else
            dwnldSpeedNew = -2; //flag for empty line
        emit enableSaveBtn();
    }
}

void SyncPage::setNewUpldSpeed()
{
    QObject *obj = this->sender();
    QString objname = obj->objectName();
    if (objname == "rBtnSyncUpldAuto")
    {
        upldSpeedNew = 0;
        win->ui->edit_UpldSpeed->setEnabled(false);
        emit enableSaveBtn();
        return;
    }
    if (objname == "rBtnSyncUpldUnlimit")
    {
        upldSpeedNew = -1;
        win->ui->edit_UpldSpeed->setEnabled(false);
        emit enableSaveBtn();
        return;
    }
    if (objname == "rbtnSyncupldChoose")
    {
        win->ui->edit_UpldSpeed->setEnabled(true);

        //already entered new value but changed after that without save
        if(!win->ui->edit_UpldSpeed->text().isEmpty())
        {
            if(upldSpeedNew != win->ui->edit_UpldSpeed->text().toInt()*1000)
                upldSpeedNew = win->ui->edit_UpldSpeed->text().toInt()*1000;
        }
        else
            upldSpeedNew = -2; //flag for empty line
        emit enableSaveBtn();
    }
}

void SyncPage::setNewSpeedFromEditline()
{   
    QObject *obj = this->sender();
    QString objname = obj->objectName();
    if (objname == "edit_UpldSpeed")
    {
        if(!win->ui->edit_UpldSpeed->text().isEmpty())
            upldSpeedNew = win->ui->edit_UpldSpeed->text().toInt()*1000;
        else
            upldSpeedNew = -2; //flag for empty line
    }
    else
    {
        if(!win->ui->edit_DwnldSpeed->text().isEmpty())
            dwnldSpeedNew = win->ui->edit_DwnldSpeed->text().toInt()*1000;
        else
            dwnldSpeedNew = -2; //flag for empty line
    }
    emit enableSaveBtn();
}

void SyncPage::enableSaveBtn()
{    
    if (dwnldSpeedNew == -2 || upldSpeedNew == -2) // if one of speeds is choosen to be custom value but the value is not entered
    {
        win->ui->btnSyncSttngsSave->setEnabled(false);
        return;
    }
    if (SSL != win->ui->checkBoxSyncSSL->isChecked()
            || p2p != win->ui->checkBoxp2p->isChecked()
            || minLocalSpace != win->ui->edit_minLocalSpace->text()
            || upldSpeed != upldSpeedNew || dwnldSpeed != dwnldSpeedNew
            || win->ui->text_patterns->document()->isModified())
    {
        win->ui->btnSyncSttngsSave->setEnabled(true);
        win->ui->btnSyncSttngCancel->setEnabled(true);
    }
    else
    {
        win->ui->btnSyncSttngsSave->setEnabled(false);
        win->ui->btnSyncSttngCancel->setEnabled(false);
    }
    clearSpeedEditLines();
}

void SyncPage::saveSettings()
{
    if (SSL != win->ui->checkBoxSyncSSL->isChecked())
    {
        SSL = !SSL;
        psync_set_bool_setting("usessl", SSL);
    }
    if (p2p != win->ui->checkBoxp2p->isChecked())
    {
        p2p = !p2p;
        psync_set_bool_setting("p2psync", p2p);
    }
    if (minLocalSpace != win->ui->edit_minLocalSpace->text())
    {
        minLocalSpace = win->ui->edit_minLocalSpace->text();
        // minLocalSpace.toInt()*1024*1024 doesnt returns right result if space = 9999mb
        quint64 n = 1024*1024;
        psync_set_uint_setting("minlocalfreespace", minLocalSpace.toInt()*n);
    }

    if(upldSpeed != upldSpeedNew)
    {
        psync_set_int_setting("maxuploadspeed",upldSpeedNew);
        upldSpeed = upldSpeedNew;
    }

    if(dwnldSpeed != dwnldSpeedNew)
    {
        psync_set_int_setting("maxdownloadspeed", dwnldSpeedNew);
        dwnldSpeed = dwnldSpeedNew;
    }

    if(patterns != win->ui->text_patterns->toPlainText())
    {
        patterns = win->ui->text_patterns->toPlainText();
        psync_set_string_setting("ignorepatterns",patterns.toUtf8());
    }
    qDebug()<<"Sync New Settings Saved: patterns= " <<patterns << " ssl=" << SSL << " minspace="<< minLocalSpace;
    qDebug()<< "lib vals space="<<(psync_get_uint_setting("minlocalfreespace"))/1024/1024 << " dwnldspeed="
            << (psync_get_int_setting("maxdownloadspeed"))/1000 << " upldspeed="
            << (psync_get_int_setting("maxuploadspeed"))/1000 << " ssl=" <<psync_get_bool_setting("usessl");
    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);
}
void SyncPage::cancelSettings()
{
    loadSettings();
    win->ui->btnCancelSttngs->setEnabled(false);
    win->ui->btnSaveSttngs->setEnabled(false);

}
void SyncPage::clearSpeedEditLines()
{
    if((dwnldSpeedNew == 0 || dwnldSpeedNew == -1) && !win->ui->edit_DwnldSpeed->text().isNull())
        win->ui->edit_DwnldSpeed->clear();
    if((upldSpeedNew == 0 || upldSpeedNew == -1)  && !win->ui->edit_UpldSpeed->text().isNull())
        win->ui->edit_UpldSpeed->clear();
}

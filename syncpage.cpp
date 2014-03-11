#include "syncpage.h"
#include "psynclib.h"
#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"
#include "modifysyncdialog.h"
#include "ui_modifysyncdialog.h"
#include <QRegExp>
#include <QDebug> //temp


SyncPage::SyncPage(PCloudWindow *w, PCloudApp *a, QWidget *parent) :
    QWidget(parent)
{
    win = w;
    app = a;

    // win->ui->label_errSync->setVisible(false);
    initSyncPage();
    win->ui->tabWidgetSync->setTabText(0, trUtf8("Synced Folders"));
    win->ui->tabWidgetSync->setTabText(1, trUtf8("Sync Settings"));
    win->ui->tabWidgetSync->setCurrentIndex(0);

    QRegExp regExp("[1-9][0-9]{0,3}");
    win->ui->edit_minLocalSpace->setValidator(new QRegExpValidator(regExp, this));
    QRegExp regExpSpeed("[1-9][0-9]{0,4}");
    win->ui->edit_DwnldSpeed->setValidator(new QRegExpValidator(regExpSpeed, this));
    win->ui->edit_UpldSpeed->setValidator(new QRegExpValidator(regExpSpeed, this));

    connect(win->ui->tabWidgetSync, SIGNAL(currentChanged(int)), this, SLOT(refreshTab(int)));
    connect(win->ui->treeSyncList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(modifySync()));
    connect(win->ui->btnSyncModify,SIGNAL(clicked()), this, SLOT(modifySync()));
    connect(win->ui->btnSyncStop, SIGNAL(clicked()), this, SLOT(stopSync()));
    connect(win->ui->btnSyncAdd, SIGNAL(clicked()), this, SLOT(addSync()));

    connect(win->ui->btnSyncSttngsSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnSyncSttngCancel, SIGNAL(clicked()), this, SLOT(cancelSettings()));
    connect(win->ui->edit_DwnldSpeed, SIGNAL(textEdited(QString)), this, SLOT(enableSaveBtn()));
    connect(win->ui->edit_UpldSpeed, SIGNAL(textEdited(QString)), this, SLOT(enableSaveBtn()));
    connect(win->ui->edit_minLocalSpace, SIGNAL(textEdited(QString)), this, SLOT(enableSaveBtn()));
    connect(win->ui->checkBoxSyncSSL, SIGNAL(stateChanged(int)), this, SLOT(enableSaveBtn()));
    connect(win->ui->text_patterns, SIGNAL(textChanged()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rbtnSyncDwnlChoose, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rBtnSyncDwldAuto, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rBtnSyncDwldUnlimit, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rbtnSyncupldChoose, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rBtnSyncUpldAuto, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));
    connect(win->ui->rBtnSyncUpldUnlimit, SIGNAL(clicked()), this, SLOT(enableSaveBtn()));

}
void SyncPage::refreshTab(int index)
{
    if (index)
        loadSettings();
    else
        load();
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
        break;
    }
}

void SyncPage::load()
{

    win->ui->treeSyncList->clear();
    psync_folder_list_t *fldrsList = psync_get_sync_list();
    if (fldrsList != NULL && fldrsList->foldercnt)
    {
        for (int i = 0; i< fldrsList->foldercnt; i++)
        {
            qDebug()<<"Load sync tab"<< fldrsList->folders[i].folderid << " "<<fldrsList->folders[i].localname<< " "
                   <<fldrsList->folders[i].localpath << " "<<fldrsList->folders[i].remotename
                  << " "<<fldrsList->folders[i].remotepath<< " sync id="<<fldrsList->folders[i].syncid<< "sync type "<<fldrsList->folders[i].synctype;

            QStringList row;
            // row << fldrsList->folders[i].localpath << QString::number(fldrsList->folders[i].synctype) << fldrsList->folders[i].remotepath;
            row << fldrsList->folders[i].localpath <<get_sync_type(fldrsList->folders[i].synctype) << fldrsList->folders[i].remotepath;
            QTreeWidgetItem *item = new QTreeWidgetItem(row);
            item->setData(0, Qt::UserRole,fldrsList->folders[i].localpath);
            item->setData(1, Qt::UserRole, fldrsList->folders[i].synctype);
            item->setData(2, Qt::UserRole, fldrsList->folders[i].remotepath);
            item->setData(3,  Qt::UserRole, fldrsList->folders[i].syncid);
            win->ui->treeSyncList->insertTopLevelItem(i, item);
        }

        free(fldrsList);
    }
}


void SyncPage::initSyncPage()
{    
    load();
    loadSettings();
}

void SyncPage::modifySync()
{
    QTreeWidgetItem *current = win->ui->treeSyncList->currentItem();
    if (!current)
    {
        win->ui->label_errSync->setText(trUtf8("Please select a sync!"));
        return;
    }
    else
    {
        //if statuses may be
        //ModifySyncDialog modifyDialog(current->data(0, Qt::UserRole).toString(), current->data(2, Qt::UserRole).toString(), current->data(1, Qt::UserRole).toString());
        ModifySyncDialog modifyDialog(current->data(0, Qt::UserRole).toString(), current->data(2, Qt::UserRole).toString(),current->data(1, Qt::UserRole).toInt());

        if (modifyDialog.exec() == QDialog::Accepted)
        {
            //if ((psync_change_synctype(current->data(3, Qt::UserRole),(modidyDialog->ui->combo_Directions->currentIndex()+1))));
            quint32 type = modifyDialog.ui->combo_Directions->currentIndex()+1;
            //quint32 syncid = current->data(3, Qt::UserRole).toInt();
            //int res = psync_change_synctype(syncid,type);
            // psync_change_synctype(syncid, type); NOT IMPLEMENTED
            //if(res)
            showError();
            modifyDialog.hide();
        }
    }
}
void SyncPage::stopSync()
{
    QTreeWidgetItem *current = win->ui->treeSyncList->currentItem();
    if (!current)
    {
        win->ui->label_errSync->setText("Please select a sync!");
        return;
    }
    else
    {
        psync_syncid_t syncid = current->data(3,Qt::UserRole).toInt();
        qDebug()<<"Stop sync with id " <<syncid;
        if (psync_delete_sync(syncid)) //0 -ok, -1 err
            showError();
        else
        {
            load();
            app->createSyncFolderActions(app->getSyncMenu());
        }
    }
}

void SyncPage::addSync()
{
    //check za statusi
    addSyncDialog *addSync = new addSyncDialog(app,win,this, NULL);
    addSync->exec(); //or show

}


// Settings tab
void SyncPage::loadSettings()
{
    SSL = psync_get_bool_setting("usessl");
    win->ui->checkBoxSyncSSL->setChecked(SSL);

    minLocalSpace = QString::number((psync_get_uint_setting("minlocalfreespace"))/1024/1024);
    win->ui->edit_minLocalSpace->setText(minLocalSpace);

    // maximum upload speed in bytes per second, 0 for auto-shaper, -1 for no limit
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

    connect(win->ui->rBtnSyncDwldAuto, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncDwldUnlimit, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rbtnSyncDwnlChoose, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncUpldAuto, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->rBtnSyncUpldUnlimit, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->rbtnSyncupldChoose, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));


    patterns = psync_get_string_setting("ignorepatterns");
    win->ui->text_patterns->setText(patterns);

    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}
void SyncPage::setNewDwnldSpeed()
{
    QObject *obj = this->sender();
    qDebug()<< "object: "<< obj->objectName();
    QString objname = obj->objectName();
    if (objname == "rBtnSyncDwldAuto")
    {
        dwnldSpeedNew = 0;
        win->ui->edit_DwnldSpeed->setEnabled(false);
        qDebug()<< dwnldSpeedNew;
        return;
    }
    if (objname == "rBtnSyncDwldUnlimit")
    {
        dwnldSpeedNew = -1;
        win->ui->edit_DwnldSpeed->setEnabled(false);
        qDebug()<< dwnldSpeedNew;
        return;
    }
    if (objname == "rbtnSyncDwnlChoose")
    {
        win->ui->edit_DwnldSpeed->setEnabled(true);
        connect(win->ui->edit_DwnldSpeed, SIGNAL(textEdited(QString)), this, SLOT(setNewSpeedFromEditline()));
        return;
    }
}

void SyncPage::setNewUpldSpeed()
{
    QObject *obj = this->sender();
    qDebug()<< "object: "<< obj->objectName();
    QString objname = obj->objectName();
    if (objname == "rBtnSyncUpldAuto")
    {
        upldSpeedNew = 0;
        win->ui->edit_UpldSpeed->setEnabled(false);
        qDebug()<< upldSpeedNew;
        return;
    }
    if (objname == "rBtnSyncUpldUnlimit")
    {
        upldSpeedNew = -1;
        win->ui->edit_UpldSpeed->setEnabled(false);
        qDebug()<< upldSpeedNew;
        return;
    }
    if (objname == "rbtnSyncupldChoose")
    {
        connect(win->ui->edit_UpldSpeed, SIGNAL(textEdited(QString)), this, SLOT(setNewSpeedFromEditline()));
        win->ui->edit_UpldSpeed->setEnabled(true);
        return;
    }
}

void SyncPage::setNewSpeedFromEditline()
{
    QObject *obj = this->sender();
    qDebug()<< "object: "<< obj->objectName();
    QString objname = obj->objectName();
    if (objname == "edit_UpldSpeed")
        upldSpeedNew = (win->ui->edit_UpldSpeed->text().toInt()) *1000 ;
    else
        dwnldSpeedNew = win->ui->edit_DwnldSpeed->text().toInt()*1000;
}

void SyncPage::enableSaveBtn()
{
    if (SSL != win->ui->checkBoxSyncSSL->isChecked()
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
}

void SyncPage::saveSettings()
{
    if (SSL != win->ui->checkBoxSyncSSL->isChecked())
    {
        SSL = !SSL;
        psync_set_bool_setting("usessl", SSL);
        qDebug()<<SSL;
    }
    if (minLocalSpace != win->ui->edit_minLocalSpace->text())
    {
        minLocalSpace = win->ui->edit_minLocalSpace->text();
        psync_set_uint_setting("minlocalfreespace", (((minLocalSpace.toInt()*1024*1024))));
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
        // qDebug()<< win->ui->text_patterns->toPlainText();
    }
    qDebug()<<"Settings Saved btn pressed: " <<patterns << " " << SSL << " "<< minLocalSpace;
    qDebug()<< "lib vals "<<(psync_get_uint_setting("minlocalfreespace"))/1024/1024 << " "
            << (psync_get_int_setting("maxdownloadspeed"))/1000 << " "
            << (psync_get_int_setting("maxuploadspeed"))/1000 << " " <<psync_get_bool_setting("usessl");
    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}
void SyncPage::cancelSettings()
{
    loadSettings();
}


void SyncPage::showError()
{
    int err = psync_get_last_error();
    switch(err)
    {
    case 1: // for us
        qDebug()<< " ERROR : Local folder not found. ";
        break;
    case 2:
        qDebug()<< "ERROR : Remote folder not found. ";
        break;
    case 3:
        qDebug()<< "ERROR :Error opening database. ";
        break;
    case 4:
        qDebug()<< "ERROR :No home directory found. ";
        break;
    case 5:
        qDebug()<< "ERROR :SSL initialization failed. ";
        break;
    case 6:
        qDebug()<< "ERROR :Database error. ";
        break;
    case 7:
        qDebug()<< "ERROR :Local folder access denied. ";
        break;
    case 8:
        qDebug()<< "ERROR :Remote folder access denied. ";
        break;
    case 9:
        qDebug()<< "ERROR :Folder already syncing. ";
        break;
    case 10:
        qDebug()<< "ERROR : Invalid synctype. ";
        break;
    case 11:
        qDebug()<< "ERROR :Sync disconnected. ";
        break;
    default:
        break;
    }
}

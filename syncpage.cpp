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
    win->ui->tabWidgetSync->setTabText(0, tr("Synced Folders"));
    win->ui->tabWidgetSync->setTabText(1, tr("Sync Settings"));
    win->ui->tabWidgetSync->setCurrentIndex(0);

    QRegExp regExp("[1-9][0-9]{0,3}");
    win->ui->edit_minLocalSpace->setValidator(new QRegExpValidator(regExp, this));
    QRegExp regExpSpeed("[1-9][0-9]{0,3}");
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

static QString get_sync_type(int synctype) // s masiv in sync.h
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
            qDebug()<<fldrsList->folders[i].folderid << " "<<fldrsList->folders[i].localname<< " "<<fldrsList->folders[i].localpath << " "<<fldrsList->folders[i].remotename
                   << " "<<fldrsList->folders[i].remotepath<< " "<<fldrsList->folders[i].syncid<< " "<<fldrsList->folders[i].synctype;

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
    // else
    //  win->ui->label_errSync->setText("Error Loading");
}


void SyncPage::initSyncPage()
{
    //if(psync_init()) //0 on success and -1 otherwise.
    //  showError();
    //  psync_set_auth(app->authentication.toUtf8(),app->rememberMe); //check has it already set
    //psync_download_state - befeore listing remote fldr, before adding// not ready at the moment
    //   psync_set_user_pass("fdfs", "fsfs", 1);
    // psync_start_sync(status_callback, event_callback);// to be logged
    //psync_start_sync(status_callback, event_callback);// to be logged
    //to add check for statuses and then load
    load();
    loadSettings();

}

void SyncPage::modifySync()
{
    QTreeWidgetItem *current = win->ui->treeSyncList->currentItem();
    if (!current)
    {
        win->ui->label_errSync->setText(tr("Please select a sync!"));
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
        if (psync_delete_sync(syncid)) //0 -ok, -1 err
            //win->ui->label_errSync->setText("Error during deleting");
            showError();
        else
            load();
    }
}
void SyncPage::pauseSync(){

}
void SyncPage::addSync()
{
    //check za statusi
    addSyncDialog *addSync = new addSyncDialog(app,win,this, NULL);
    addSync->exec(); //or show

}

void SyncPage::loadSettings()
{
    SSL = psync_get_bool_setting("usessl");
    win->ui->checkBoxSyncSSL->setChecked(SSL);
  //  QString radio = win->ui->rBtnSyncUpldAuto->accessibleName();
  //  qDebug()<<" widget name : "<<radio;
    minLocalSpace =QString::number((psync_get_uint_setting("minlocalfreespace"))/1024.0/1024.0);
    win->ui->edit_minLocalSpace->setText(minLocalSpace);

    // maximum upload speed in bytes per second, 0 for auto-shaper, -1 for no limit
    //donwload default - unlimitted -1
    //upload default - auto-shater 0
    int dwnldSpeed = psync_get_int_setting("maxdownloadspeed");
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
            maxDwnldSpeed = QString::number(dwnldSpeed/1000);
            win->ui->edit_DwnldSpeed->setText(maxDwnldSpeed);
            win->ui->rbtnSyncDwnlChoose->setEnabled(true);
        }
    }

    int uploadSpeed = psync_get_int_setting("maxuploadspeed");
    if (!uploadSpeed)
    {
        win->ui->rBtnSyncUpldAuto->setChecked(true);
        // win->ui->rBtnSyncUpldUnlimit->setChecked(false);
        win->ui->edit_UpldSpeed->setEnabled(false);
    }
    else
    {
        if (uploadSpeed == -1)
        {
            win->ui->rBtnSyncUpldUnlimit->setChecked(true);
            //    win->ui->rBtnSyncUpldAuto->setChecked(false);
            win->ui->edit_UpldSpeed->setEnabled(false);
        }
        else
        {
            maxDwnldSpeed = QString::number(uploadSpeed/1000);
            win->ui->edit_UpldSpeed->setText(maxDwnldSpeed);
            //win->ui->rBtnSyncUpldUnlimit->setChecked(false);
            //  win->ui->rBtnSyncUpldAuto->setChecked(false);
            win->ui->edit_UpldSpeed->setEnabled(true);
        }
    }
    connect(win->ui->rbtnSyncupldChoose,SIGNAL(clicked(bool)), win->ui->edit_UpldSpeed, SLOT(setEnabled(bool)));
    connect(win->ui->rbtnSyncDwnlChoose, SIGNAL(clicked(bool)), win->ui->edit_DwnldSpeed, SLOT(setEnabled(bool)));


    //maxUpldSpeed = QString::number((psync_get_int_setting("maxuploadspeed"))/1000);
    //win->ui->edit_UpldSpeed->setText(maxUpldSpeed);

    patterns = psync_get_string_setting("ignorepatterns");
    win->ui->text_patterns->setText(patterns);
   // qDebug()<<patterns << " " << SSL << " "<< minLocalSpace <<" "<< maxDwnldSpeed << " " << " " <<maxUpldSpeed;

    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}
//void SyncPage::editLineEnable(bool b)
//{

//}

void SyncPage::enableSaveBtn()
{
    /*if (SSL != win->ui->checkBoxSyncSSL->isChecked()
            || minLocalSpace != win->ui->edit_minLocalSpace->text() ||
            maxDwnldSpeed != win->ui->edit_DwnldSpeed->text() || maxUpldSpeed != win->ui->edit_UpldSpeed->text()
            || win->ui->text_patterns->document()->isModified())
    {
        win->ui->btnSyncSttngsSave->setEnabled(true);
        win->ui->btnSyncSttngCancel->setEnabled(true);
    }
    else
    {
        win->ui->btnSyncSttngsSave->setEnabled(false);
        win->ui->btnSyncSttngCancel->setEnabled(false);
    }*/

    win->ui->btnSyncSttngsSave->setEnabled(true);
    win->ui->btnSyncSttngCancel->setEnabled(true);
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
        psync_set_uint_setting("minlocalfreespace", (((minLocalSpace.toDouble()*1024*1024*1024))));
    }

    if (maxUpldSpeed != win->ui->edit_UpldSpeed->text())
    {
        maxUpldSpeed = win->ui->edit_UpldSpeed->text();
        psync_set_int_setting("maxuploadspeed", ((maxUpldSpeed.toInt())*1000));
    }

    if (maxDwnldSpeed != win->ui->edit_DwnldSpeed->text())
    {
        maxDwnldSpeed = win->ui->edit_DwnldSpeed ->text();
        psync_set_int_setting("maxdownloadspeed", ((maxDwnldSpeed.toInt())*1000));
    }

    if(patterns != win->ui->text_patterns->toPlainText())
    {
        patterns = win->ui->text_patterns->toPlainText();
        psync_set_string_setting("ignorepatterns",patterns.toUtf8());
        // qDebug()<< win->ui->text_patterns->toPlainText();
    }
    qDebug()<<"Saved: " <<patterns << " " << SSL << " "<< minLocalSpace <<" "<< maxDwnldSpeed << " " << " " <<maxUpldSpeed;
    qDebug()<< (psync_get_uint_setting("minlocalfreespace"))/1024.0/1024.0/1024.0 << " "
            << (psync_get_int_setting("maxdownloadspeed"))/1000 << " "
            << (psync_get_int_setting("maxuploadspeed"))/1000 << " " <<psync_get_bool_setting("usessl");
    win->ui->btnSyncSttngsSave->setEnabled(false);
    win->ui->btnSyncSttngCancel->setEnabled(false);

}
void SyncPage::cancelSettings()
{
    loadSettings();
}
void SyncPage::refreshTab(int index)
{
    if (index)
        loadSettings();
}

void SyncPage::showError() //static may be
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

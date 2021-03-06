#include "settingspage.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#include <qt_windows.h>
#include <qwindowdefs_win.h>
#endif

// this class will be finalized as behaviour when add vfs settings

SettingsPage::SettingsPage(PCloudWindow *w, PCloudApp *a, QObject* parent):
    QObject(parent)
{
    win = w;
    app = a;
    win->ui->tabWidgetSttngs->setTabText(0,trUtf8("General"));
    win->ui->tabWidgetSttngs->setTabText(1,trUtf8("Speed"));
    win->ui->tabWidgetSttngs->setTabText(2,trUtf8("Disk Usage"));
    win->ui->tabWidgetSttngs->setTabText(3,trUtf8("Crypto"));
#ifndef Q_OS_WIN
    win->ui->label_infoupld->setFont(app->smaller2pFont);
    win->ui->label_infodwld->setFont(app->smaller2pFont);
    win->ui->label_infop2p->setFont(app->smaller2pFont);
    win->ui->label_infominspace->setFont(app->smaller1pFont);
    win->ui->label_infocache->setFont(app->smaller1pFont);
    win->ui->groupBoxCryptoLock->setTitle("Lock Crypto Folder on Suspend mode");
    win->ui->checkBoxLockCrypto->setText("Automatically lock the Crypto Folder when your device goes to Suspend or Hibernate mode.");
#else
    win->ui->line_settngsGeneral->setVisible(false);
    win->ui->line_settngsSpace->setVisible(false);
    win->ui->line_settngsSpeed1->setVisible(false);
    win->ui->line_settngsSpeed2->setVisible(false);
    win->ui->line_crypto1->setVisible(false);
    win->ui->line_crypto2->setVisible(false);
#endif
    win->ui->label_infop2p->setText("pCloud Drive uses p2p (peer to peer), which is a computer-to-computer connection within your\nLocal Area Network (LAN),to speed up synchronization.");

    QRegExp regExp("[1-9][0-9]{0,4}");
    QRegExpValidator* regExpValidator = new QRegExpValidator(regExp, this);
    win->ui->edit_DwnldSpeed->setValidator(regExpValidator);
    win->ui->edit_UpldSpeed->setValidator(regExpValidator);

    QRegExp regExpSpace("[1-9][0-9]{0,5}");
    QRegExpValidator* regExpValidatornewSpace = new QRegExpValidator(regExpSpace, this);
    win->ui->edit_minLocalSpace->setValidator(regExpValidatornewSpace);
    win->ui->edit_cache->setValidator(regExpValidatornewSpace);

    connect(win->ui->checkBoxp2p, SIGNAL(clicked()), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->rBtnSyncDwldAuto, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncDwldUnlimit, SIGNAL(clicked()),this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->edit_DwnldSpeed, SIGNAL(textEdited(QString)), this, SLOT(setNewDwnldSpeed()));
    connect(win->ui->rBtnSyncUpldAuto, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->rBtnSyncUpldUnlimit, SIGNAL(clicked()),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->edit_UpldSpeed, SIGNAL(textEdited(QString)),this, SLOT(setNewUpldSpeed()));
    connect(win->ui->edit_cache, SIGNAL(textEdited(QString)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->edit_minLocalSpace, SIGNAL(textEdited(QString)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBox_autorun, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBoxAutoAskCrypto, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBoxLockCrypto, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->btnCryptoResetKey, SIGNAL(clicked()), this, SLOT(resetCryptoKey()));

#ifdef Q_OS_WIN
    connect(win->ui->checkBox_contxtMenu, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    win->ui->tabWidgetSttngs->setMaximumHeight(300);
#define SETTNGS_CRYPTO_TAB_NUM 3
#else
    win->ui->tabWidgetSttngs->removeTab(0);
#define SETTNGS_CRYPTO_TAB_NUM 2
#endif
    connect(win->ui->btnSaveSttngs, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnCancelSttngs, SIGNAL(clicked()), this, SLOT(cancelSettings()));

}

void SettingsPage::initPublic()
{
    this->initSettingsPage();
}

void SettingsPage::showEvent()
{
    win->ui->tabWidgetSttngs->setCurrentIndex(0);
    if (win->ui->btnSaveSttngs->isEnabled())
    {
        win->ui->btnSaveSttngs->setEnabled(false);
        win->ui->btnCancelSttngs->setEnabled(false);
        this->initSettingsPage();
    }
    if(!app->isCryptoExpired)
        win->ui->tabWidgetSttngs->setTabEnabled(SETTNGS_CRYPTO_TAB_NUM, true);
    else
        win->ui->tabWidgetSttngs->setTabEnabled(SETTNGS_CRYPTO_TAB_NUM, false);
}

void SettingsPage::initSettingsPage()
{    
    initMain();
    initSpeed();
    initSpace();
    if(!app->isCryptoExpired)
    {
        initCrypto();
        win->ui->tabWidgetSttngs->setTabEnabled(SETTNGS_CRYPTO_TAB_NUM, true);
    }
    else
        win->ui->tabWidgetSttngs->setTabEnabled(SETTNGS_CRYPTO_TAB_NUM, false);

    clearSpeedEditLines();
    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
}

void SettingsPage::initMain()
{
#ifdef Q_OS_WIN
    //autostart app
    win->ui->checkBox_autorun->setChecked(app->registrySttng->contains("pCloud"));

    //context menu
    if(app->settings->contains("shellExt"))
        win->ui->checkBox_contxtMenu->setChecked(app->settings->value("shellExt").toBool());
    else
    {
        win->ui->checkBox_contxtMenu->setChecked(true);
        app->settings->setValue("shellExt", true);
    }
#endif
}

void SettingsPage::initSpeed()
{
    //maximum upload speed in bytes per second, 0 for auto-shaper, -1 for no limit
    //donwload default - unlimitted -1
    //upload default - auto-shater 0

    dwnldSpeed = psync_get_int_setting("maxdownloadspeed");
    dwnldSpeedNew = dwnldSpeed;
    if (!dwnldSpeed)
    {
        win->ui->rBtnSyncDwldAuto->setChecked(true);
        win->ui->rbtnSyncDwnlChoose->setEnabled(false);
    }
    else if (dwnldSpeed == -1)
    {
        win->ui->rBtnSyncDwldUnlimit->setChecked(true);
        win->ui->rbtnSyncDwnlChoose->setEnabled(false);
    }
    else
    {
        win->ui->rbtnSyncDwnlChoose->setChecked(true);
        win->ui->edit_DwnldSpeed->setText(QString::number(dwnldSpeed/1000));
    }

    upldSpeed = psync_get_int_setting("maxuploadspeed");
    upldSpeedNew = upldSpeed;
    if (!upldSpeed)
    {
        win->ui->rBtnSyncUpldAuto->setChecked(true);
        win->ui->rbtnSyncupldChoose->setEnabled(false);
    }
    else if (upldSpeed == -1)
    {
        win->ui->rBtnSyncUpldUnlimit->setChecked(true);
        win->ui->rbtnSyncupldChoose->setEnabled(false);
    }
    else
    {
        win->ui->rbtnSyncupldChoose->setChecked(true);
        win->ui->edit_UpldSpeed->setText(QString::number(upldSpeed/1000));
    }

    p2p = psync_get_bool_setting("p2psync");
    win->ui->checkBoxp2p->setChecked(p2p);

}

void SettingsPage::initSpace()
{
    cacheSize = psync_get_uint_setting("fscachesize") >> 20;
    win->ui->edit_cache->setText(QString::number(cacheSize));

    minLocalSpace = (psync_get_uint_setting("minlocalfreespace")) >> 20;
    win->ui->edit_minLocalSpace->setText(QString::number(minLocalSpace));
}

void SettingsPage::initCrypto()
{
    qDebug()<<"SettingsPage::initCrypto";
    lockCryptoFldr = psync_get_bool_setting("sleepstopcrypto");
    win->ui->checkBoxLockCrypto->setChecked(lockCryptoFldr);

    if(app->settings->contains("autostartcrypto"))
        autoaskCryptoKey = app->settings->value("autostartcrypto").toBool();
    else //first init - by default this setting is off
    {
        autoaskCryptoKey = false;
        app->settings->setValue("autostartcrypto",false);
    }
    win->ui->checkBoxAutoAskCrypto->setChecked(autoaskCryptoKey);

}

//slots
void SettingsPage::setNewDwnldSpeed()
{
    QObject *obj = this->sender();
    QString objname = obj->objectName();

    if (objname == "rBtnSyncDwldAuto")
    {
        dwnldSpeedNew = 0;
        win->ui->rbtnSyncDwnlChoose->setEnabled(false);
    }
    else if (objname == "rBtnSyncDwldUnlimit")
    {
        dwnldSpeedNew = -1;
        win->ui->rbtnSyncDwnlChoose->setEnabled(false);
    }
    else if (objname == "edit_DwnldSpeed")
    {
        win->ui->rbtnSyncDwnlChoose->setEnabled(true);
        win->ui->rbtnSyncDwnlChoose->setChecked(true);

        //already entered new value but changed after that without save
        if(!win->ui->edit_DwnldSpeed->text().isEmpty())
            dwnldSpeedNew = win->ui->edit_DwnldSpeed->text().toInt()*1000;
        else
            dwnldSpeedNew = -2; //flag for empty line
    }
    if(dwnldSpeed != dwnldSpeedNew)
        emit setSaveBtnEnable();

}

void SettingsPage::setNewUpldSpeed()
{

    QObject *obj = this->sender();
    QString objname = obj->objectName();

    if (objname == "rBtnSyncUpldAuto")
    {
        upldSpeedNew = 0;
        win->ui->rbtnSyncupldChoose->setEnabled(false);
    }
    else if (objname == "rBtnSyncUpldUnlimit")
    {
        upldSpeedNew = -1;
        win->ui->rbtnSyncupldChoose->setEnabled(false);
    }
    else if (objname == "edit_UpldSpeed")
    {
        win->ui->rbtnSyncupldChoose->setEnabled(true);
        win->ui->rbtnSyncupldChoose->setChecked(true);

        //already entered new value but changed after that without save
        if(!win->ui->edit_UpldSpeed->text().isEmpty())
        {
            // if(dwnldSpeedNew != win->ui->edit_DwnldSpeed->text().toInt()*1000)
            upldSpeedNew = win->ui->edit_UpldSpeed->text().toInt()*1000;
        }
        else
        {
            upldSpeedNew = -2; //flag for empty line
        }
    }
    if(upldSpeed != upldSpeedNew)
        emit setSaveBtnEnable();
}

qint32 SettingsPage::getCacheSize()
{
    /*
#ifdef Q_OS_WIN
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    quint64 totalPhysRAM = (quint64)status.ullTotalPhys/1024/1024/10;
    quint64 totalAvailRAM = (quint64)status.ullAvailPhys/1024/1024;
#else
    struct sysinfo sys_info;
    sysinfo(&sys_info);
    quint64  totalPhysRAM=(quint64)(sys_info.totalram/1024/1024/10);
    quint64  totalAvailRAM=(quint64)(sys_info.freeram/1024/1024);
#endif

    quint64 totalRAM = (totalPhysRAM < totalAvailRAM)? totalPhysRAM:totalAvailRAM ;
    // return totalRAM;
    */
    return 10; //temp
}

void SettingsPage::resetCryptoKey()
{
    if(psync_crypto_isstarted())
    {
        QMessageBox::critical(win,trUtf8("Resetting Passphrase"), trUtf8("Please lock your folder first!"));
        return;
    }

    if (QMessageBox::Yes == QMessageBox::warning(win, trUtf8("Resetting Passphrase"), trUtf8("By resetting your Passphrase you will delete all currently encrypted files. Are you sure?"),
                                                 QMessageBox::Yes|QMessageBox::Cancel))
    {
        qDebug()<<"Crypto: reset settings";

        int resetCrRes = psync_crypto_reset();
        if(!resetCrRes)
        {
            QMessageBox::information(win,"Resetting Passphrase",
                                     trUtf8("An email with instructions has been sent to %1. Please, follow the steps to reset your Passphrase.").arg(app->username));

            if (app->settings->value("autostartcrypto").toBool())
            {
                autoaskCryptoKey = false;
                app->settings->setValue("autostartcrypto", autoaskCryptoKey);
                win->ui->checkBoxAutoAskCrypto->setChecked(false);
            }
            win->getCryptoPage()->clearSetupUI();
        }
        else
            showResetError(resetCrRes);
    }
}

void SettingsPage::saveSettings()
{   
    if(p2p != win->ui->checkBoxp2p->isChecked())   //p2p
    {
        p2p = !p2p;
        psync_set_bool_setting("p2psync", p2p);
    }

    if(dwnldSpeed != dwnldSpeedNew)
    {
        psync_set_int_setting("maxdownloadspeed", dwnldSpeedNew);
        dwnldSpeed = dwnldSpeedNew;
    }

    if(upldSpeed != upldSpeedNew)
    {
        psync_set_int_setting("maxuploadspeed", upldSpeedNew);
        upldSpeed = upldSpeedNew;
    }

    if(cacheSize != win->ui->edit_cache->text().toUInt())  //cache
    {
        if(win->ui->edit_cache->text().toUInt() > 256)
            cacheSize = win->ui->edit_cache->text().toUInt();
        else
        {
            cacheSize = 256;
            win->ui->edit_cache->setText(QString::number(256));
        }

        psync_set_uint_setting("fscachesize",cacheSize<<20);
    }

    if (minLocalSpace != win->ui->edit_minLocalSpace->text().toUInt()) //min local space
    {
        minLocalSpace = win->ui->edit_minLocalSpace->text().toUInt();
        psync_set_uint_setting("minlocalfreespace", minLocalSpace << 20 );
    }

    if(lockCryptoFldr != win->ui->checkBoxLockCrypto->isChecked())
    {
        lockCryptoFldr = !lockCryptoFldr;
        psync_set_bool_setting("sleepstopcrypto", lockCryptoFldr);
    }

    if (autoaskCryptoKey != win->ui->checkBoxAutoAskCrypto->isChecked())
    {
        autoaskCryptoKey = !autoaskCryptoKey;
        app->settings->setValue("autostartcrypto", autoaskCryptoKey);
    }

#ifdef Q_OS_WIN
    //autorun win
    if(app->registrySttng->contains("pCloud") != win->ui->checkBox_autorun->isChecked())
    {
        if(win->ui->checkBox_autorun->isChecked())
        {
            QSettings appDir("HKEY_LOCAL_MACHINE\\SOFTWARE\\PCloud\\pCloud",QSettings::NativeFormat); //take app install ddir
            app->registrySttng->setValue("pCloud",appDir.value("Install_Dir").toString().append("\\pCloud.exe"));
        }
        else
        {
            app->registrySttng->remove("pCloud");
        }
    }
    //context menu win
    if(app->settings->value("shellExt").toBool() != win->ui->checkBox_contxtMenu->isChecked())
    {
        QSettings appDir("HKEY_LOCAL_MACHINE\\SOFTWARE\\PCloud\\pCloud",QSettings::NativeFormat);
        QString cmd;
        bool isMenuReg = win->ui->checkBox_contxtMenu->isChecked();
        if(isMenuReg)
            cmd.append("regsvr32  /s \"" + appDir.value("Install_Dir").toString().append("\\SimpleExt.dll\""));
        else
            cmd.append("regsvr32 /u /s \"" + appDir.value("Install_Dir").toString().append("\\SimpleExt.dll\""));

        QProcess::execute(cmd); // register/unregister shell extensions
        app->settings->setValue("shellExt",isMenuReg);

    }
#endif

    clearSpeedEditLines();
    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
}

void SettingsPage::setSaveBtnEnable()
{
    if (dwnldSpeedNew == -2 || upldSpeedNew == -2) // if one of speeds is choosen to be custom value but the value is not entered
    {
        win->ui->btnSaveSttngs->setEnabled(false);
        win->ui->btnCancelSttngs->setEnabled(true);
        return;
    }

#ifdef Q_OS_WIN
    if(app->registrySttng->contains("pCloud") != win->ui->checkBox_autorun->isChecked() ||
            app->settings->value("shellExt").toBool() != win->ui->checkBox_contxtMenu->isChecked())
    {
        win->ui->btnSaveSttngs->setEnabled(true);
        win->ui->btnCancelSttngs->setEnabled(true);
    }

#endif      
    if ( upldSpeed != upldSpeedNew || dwnldSpeed != dwnldSpeedNew
         || p2p != win->ui->checkBoxp2p->isChecked()
         || minLocalSpace != win->ui->edit_minLocalSpace->text().toUInt()
         || cacheSize != win->ui->edit_cache->text().toUInt()
         || autoaskCryptoKey != win->ui->checkBoxAutoAskCrypto->isChecked()
         || lockCryptoFldr != win->ui->checkBoxLockCrypto->isChecked())
    {
        win->ui->btnSaveSttngs->setEnabled(true);
        win->ui->btnCancelSttngs->setEnabled(true);
    }

    clearSpeedEditLines();
}

void SettingsPage::cancelSettings()
{
    initSettingsPage();
}

void SettingsPage::clearSpeedEditLines()
{
    if((dwnldSpeedNew == 0 || dwnldSpeedNew == -1) && !win->ui->edit_DwnldSpeed->text().isNull())
        win->ui->edit_DwnldSpeed->clear();
    if((upldSpeedNew == 0 || upldSpeedNew == -1)  && !win->ui->edit_UpldSpeed->text().isNull())
        win->ui->edit_UpldSpeed->clear();
}

void SettingsPage::showResetError(int resetRes)
{
    qDebug()<<"CRYPTO: Reset res = " << resetRes;
    switch(resetRes)
    {
    case PSYNC_CRYPTO_RESET_CRYPTO_IS_STARTED:
        //QMessageBox::critical(win,"Crypto Error", "Not supported."); TO DISCUSS this case behavior!!!!
        break;
    case PSYNC_CRYPTO_RESET_CANT_CONNECT:
        QMessageBox::critical(win,"Crypto Error", "Unable to connect to server.");
        break;
    case PSYNC_CRYPTO_RESET_NOT_LOGGED_IN:
        QMessageBox::critical(win,"Crypto Error", "Your are not logged in!");
        break;
    case PSYNC_CRYPTO_RESET_NOT_SETUP:
        QMessageBox::critical(win,"Crypto Error", "The Crypto Folder is not setted up.");
        break;
    case PSYNC_CRYPTO_RESET_UNKNOWN_ERROR:
        QMessageBox::critical(win,"Crypto Error", "Unknown error.");
        break;
    default:
        break;
    }
}

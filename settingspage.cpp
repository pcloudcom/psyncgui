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

    initSettingsPage();

    //if win make visible context menu groupbox
    QRegExp regExp("[1-9][0-9]{0,6}");
    win->ui->edit_cache->setValidator(new QRegExpValidator(regExp, this));

    connect(win->ui->edit_cache, SIGNAL(textEdited(QString)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBox_onoffFs, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBox_autorun, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
#ifdef Q_OS_WIN
    connect(win->ui->checkBox_contxtMenu, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
#endif
    connect(win->ui->btnSaveSttngs, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnCancelSttngs, SIGNAL(clicked()), this, SLOT(cancelSettings()));

    //temp - VFS settings
    win->ui->groupBox_cache->setVisible(false);
    win->ui->groupBox_fldr->setVisible(false);
    win->ui->groupBox_startStop->setVisible(false);
    qDebug()<<app->settings->fileName();
}

void SettingsPage::initSettingsPage()
{
    if (!app->settings->contains("startfs"))
    {
        win->ui->checkBox_onoffFs->setCheckState(Qt::Checked);
        app->settings->setValue("startfs", true);
    }
    else
        win->ui->checkBox_onoffFs->setChecked(app->settings->value("startfs").toBool());

    if (!app->settings->contains("cachesize"))
    {
        cacheSize = getCacheSize();
        app->settings->setValue("cachesize", cacheSize);
        win->ui->edit_cache->setText(QString::number(cacheSize));
    }
    else
        win->ui->edit_cache->setText(QString::number(app->settings->value("cachesize").toInt()));

#ifdef Q_OS_WIN    
    win->ui->checkBox_autorun->setChecked(app->registrySttng->contains("pSync"));

    if(app->settings->contains("shellExt"))
        win->ui->checkBox_contxtMenu->setChecked(app->settings->value("shellExt").toBool());
    else
    {
        win->ui->checkBox_contxtMenu->setChecked(true);
        app->settings->setValue("shellExt", true);
    }
#endif

    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
}
qint32 SettingsPage::getCacheSize(){
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

void SettingsPage::dirChange() //NOT for win;  to add choose mount letter for win
{

}

void SettingsPage::saveSettings()
{
    //start/stop fs
    if(app->settings->value("startfs").toBool() != win->ui->checkBox_onoffFs->isChecked())
    {
        bool startfs = win->ui->checkBox_onoffFs->isChecked();
        app->settings->setValue("startfs",startfs);
        if (startfs)
            //psync_fs_start();
            qDebug()<< "startfs"; //temp
        else
            //psync_fs_stop();
            qDebug()<<"stopfs"; // temp
        app->settings->setValue("startfs", startfs);
    }
    //cache
    if(cacheSize != win->ui->edit_cache->text().toInt())
    {
        cacheSize = win->ui->edit_cache->text().toInt();
        app->settings->setValue("cachesize", cacheSize);
    }
#ifdef Q_OS_WIN
    //autorun win
    if(app->registrySttng->contains("pSync") != win->ui->checkBox_autorun->isChecked())
    {
        if(win->ui->checkBox_autorun->isChecked())
        {
            QSettings appDir("HKEY_LOCAL_MACHINE\\SOFTWARE\\PCloud\\pCloud",QSettings::NativeFormat); //take app install ddir
            app->registrySttng->setValue("pSync",appDir.value("Install_Dir").toString().append("\\pSync.exe"));
        }
        else
            app->registrySttng->remove("pSync");
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

    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
}

// slots
void SettingsPage::setSaveBtnEnable()
{
#ifdef Q_OS_WIN
    if(app->registrySttng->contains("pSync") != win->ui->checkBox_autorun->isChecked() ||
            //  cacheSize != win->ui->edit_cache->text().toInt() ||
            //  app->settings->value("startfs").toBool() != win->ui->checkBox_onoffFs->isChecked() ||
            app->settings->value("shellExt").toBool() != win->ui->checkBox_contxtMenu->isChecked())
    {
        win->ui->btnSaveSttngs->setEnabled(true);
        win->ui->btnCancelSttngs->setEnabled(true);
    }
    else
#endif
    {
        win->ui->btnSaveSttngs->setEnabled(false);
        win->ui->btnCancelSttngs->setEnabled(false);
    }
}

void SettingsPage::cancelSettings()
{
    initSettingsPage();
}


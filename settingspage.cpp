#include "settingspage.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"


SettingsPage::SettingsPage(PCloudWindow *w, PCloudApp *a, QObject* parent):
    QObject(parent)
{
    win = w;
    app = a;


    initSettingsPage();

    connect(win->ui->checkBox_onoff, SIGNAL(stateChanged(int)), this, SLOT(startFS(int)));
    connect(win->ui->btnSaveSttngs, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnCancelSttngs, SIGNAL(clicked()), this, SLOT(cancelSettings()));
}

void SettingsPage::initSettingsPage()
{
    win->ui->checkBox_onoff->setCheckState(Qt::Checked);
    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
}

void SettingsPage::dirChange() //NOT for win;  to add choose mount letter for win
{

}

void SettingsPage::saveSettings()
{

}


void SettingsPage::setSaveBtnEnable()
{


}

void SettingsPage::cancelSettings()
{
    initSettingsPage();
}

// slots
void SettingsPage::startFS(int n)
{
    if (n)
        psync_fs_start();
    else
        psync_fs_stop();
}

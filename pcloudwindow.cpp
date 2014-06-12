#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "changepassdialog.h"
#include "ui_changepassdialog.h"
#include "common.h"
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

    // the window consists of QListWidget(for icon-buttons) and
    //QStackedWidget which loads different page according to selected item in the listWidget (and hides other pages)

    ui->listButtonsWidget->setViewMode(QListView::IconMode);
    ui->listButtonsWidget->setFlow(QListWidget::LeftToRight); //orientation
    ui->listButtonsWidget->setSpacing(12);

    ui->listButtonsWidget->setStyleSheet("background-color:transparent");
    ui->listButtonsWidget->setMovement(QListView::Static); //not to move items with the mouse
    //temp
    ui->listButtonsWidget->setMinimumWidth(360);
    ui->listButtonsWidget->setMaximumHeight(85);
    ui->listButtonsWidget->setMinimumHeight(84); // precakva mi layouta

    //create Items for QListWidget
    new QListWidgetItem(QIcon(":/128x128/images/128x128/user.png"),trUtf8("Account"),ui->listButtonsWidget); //index 0
    new QListWidgetItem(QIcon(":/128x128/images/128x128/user.png"),trUtf8("Account"),ui->listButtonsWidget); //index 1
    new QListWidgetItem(QIcon(":/images/images/shares.png"),trUtf8("Shares"),ui->listButtonsWidget); //index 2
    new QListWidgetItem(QIcon(":/128x128/images/128x128/sync.png"),trUtf8("Sync"),ui->listButtonsWidget); //Sync Page index 3
    new QListWidgetItem(QIcon(":/images/images/settings.png"),trUtf8("Settings"),ui->listButtonsWidget); //index 4
    new QListWidgetItem(QIcon(":/128x128/images/128x128//help.png"),trUtf8("Help"),ui->listButtonsWidget); //index 5
    new QListWidgetItem(QIcon(":/128x128/images/128x128/info.png"),trUtf8("About"),ui->listButtonsWidget); //index 6

    fillAcountNotLoggedPage();
    fillAboutPage();
    settngsPage = new SettingsPage(this, app);
    syncPage = new SyncPage(this, app);
    // indexes of Items in listWidget and their coresponding pages in StackWidget are the same
    connect(ui->listButtonsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->btnVerify, SIGNAL(clicked()), this, SLOT(verifyEmail()));
    //p connect(ui->btnShareFolder,SIGNAL(clicked()), app, SLOT(shareFolder()));

    //for resize
    for(int i = 0; i < ui->pagesWidget->count(); i++)
        ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Sync");
    // ui notes - set statusbar max size to (0,0)

    connect(ui->btnLogin, SIGNAL(clicked()),app, SLOT(showLogin()));
    connect(ui->btnRegstr, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->btnExit, SIGNAL(clicked()), app, SLOT(doExit()));
    connect(ui->toolBtnContact, SIGNAL(clicked()), this, SLOT(contactUs()));
    connect(ui->tBtnOnlineHelp, SIGNAL(clicked()), this, SLOT(openOnlineHelp()));
    connect(ui->tBtnOnlineTutorial, SIGNAL(clicked()), this, SLOT(openOnlineTutorial()));
    connect(ui->tBtnFeedback, SIGNAL(clicked()), this, SLOT(sendFeedback()));
    connect(ui->toolBtnOpenWeb, SIGNAL(clicked()), this, SLOT(openWebPage()));
    connect(ui->tbtnMyPcloud, SIGNAL(clicked()), this, SLOT(openMyPcloud()));
    connect(ui->label, SIGNAL(linkActivated(QString)), this, SLOT(upgradePlan()));
    connect(ui->comboBox_versionReminder, SIGNAL(currentIndexChanged(int)), app, SLOT(setTimer(int)));
    connect(ui->btnUpdtVersn, SIGNAL(clicked()), this, SLOT(updateVersion()));
    //p connect(ui->tbtnOpenFolder, SIGNAL(clicked()),app,SLOT(openCloudDir()));
    connect(ui->tBtnExit, SIGNAL(clicked()), app, SLOT(doExit()));
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

}

PCloudWindow::~PCloudWindow()
{
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
            ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    ui->pagesWidget->widget(currentIndex)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    updateGeometry();
    ui->pagesWidget->setCurrentIndex(currentIndex); // sets page
    refreshPages();

}
void PCloudWindow::showEvent(QShowEvent *)
{
    refreshPages();
}

void PCloudWindow::refreshPages()
{
    int currentIndex = ui->listButtonsWidget->currentRow();
    if (currentIndex == 1 && verifyClicked) // Account page, case when user just has clicked Verify Email
    {
        checkVerify();
        return;
    }
    /*fs+sync if (currentIndex == 2) //shares page
    {
        sharesPage->load(0);
        return;
    }
    if (currentIndex == 4) //settings page, if user has changed smthng but hasn't save it
    {
        settngsPage->initSettingsPage();
        return;
    }
    */
}

void PCloudWindow::showpcloudWindow(int index)
{
    ui->listButtonsWidget->setCurrentRow(index);
    ui->pagesWidget->setCurrentIndex(index);

    this->raise();
    this->activateWindow();
    this->showNormal();
    this->setWindowState(Qt::WindowActive);
    app->setActiveWindow(this);
}

void PCloudWindow::setOnlineItems(bool online) // change pcloud window menu when is loggedin and loggedout
{
    if(online)
    {
        ui->listButtonsWidget->setRowHidden(0,true); //Accont - not logged
        ui->listButtonsWidget->setRowHidden(1,false); //Account - logged
        //p ui->listButtonsWidget->setRowHidden(2,false); //Shares
        ui->listButtonsWidget->setRowHidden(2,true); //Shares - false for fs = visible
        ui->listButtonsWidget->setRowHidden(3,false); //Sync
        //ui->listButtonsWidget->setRowHidden(4, false); // pcloud Settings;
    }
    else
    {
        ui->listButtonsWidget->setRowHidden(0,false);
        ui->listButtonsWidget->setRowHidden(1,true);
        ui->listButtonsWidget->setRowHidden(2,true);
        ui->listButtonsWidget->setRowHidden(3,true);
        //ui->listButtonsWidget->setRowHidden(4, false); // for fs -> false
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

void PCloudWindow::fillAboutPage()
{

    if(!app->new_version())
    {
        ui->label_versionVal->setText(QString("Version ") + APP_VERSION + QString("\n\nEverything up to date"));
        ui->label_versionVal->setAlignment(Qt::AlignHCenter);
        ui->widget_newVersion->setVisible(false);
    }
    else
    {
        ui->label_versionVal->setText(QString ("Version ") + APP_VERSION);
        ui->label_newVersion->setText(QString("New version "  + app->newVersion.versionstr + " has already been released"));
        ui->label_notes->setText(QString("Notes:\n "+ app->newVersion.notes));
        if(app->settings->contains("vrsnNotifyComboCurrIndx")) // the reminder is already set
        {
            ui->comboBox_versionReminder->setCurrentIndex(app->settings->value("vrsnNotifyComboCurrIndx").toInt());
        }
        else
            app->settings->setValue("vrsnNotifyComboCurrIndx",1);
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
    ui->label_space->setText(QString::number(app->usedSpace, 'f', 1) + "GB (" + QString::number(app->freeSpacePercentage) + "% free)");
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
    // ui->tBtnExit->setVisible(false); ?

    ui->tbtnOpenFolder->setVisible(false);
    ui->tBtnExit->setVisible(false);

}
void PCloudWindow::refreshUserinfo()
{
    this->fillAccountLoggedPage();
}

SyncPage* PCloudWindow::get_sync_page()
{
    return this->syncPage;
}

//SLOTS
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
    app->unlinkFlag = true;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,trUtf8("Unlink"), trUtf8("If You unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        psync_unlink();
        emit app->logOut(); //sets offlineimtes too
        emit setOnlineItems(false);
        app->setFirstLaunch(true);

    }
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
        if(res == -1 )
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
    QUrl url(app->newVersion.url);
    QDesktopServices::openUrl(url);
}

void PCloudWindow::openWebPage()
{
    QUrl url("https://www.pcloud.com/");
    QDesktopServices::openUrl(url);
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

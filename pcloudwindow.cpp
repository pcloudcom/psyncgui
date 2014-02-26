#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "changepassdialog.h"
#include "common.h"
#include <QDesktopServices>
#include <QUrl>
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
    //ui->listButtonsWidget->setWrapping(true); //
    //ui->listButtonsWidget->setSelectionRectVisible(false); //
    ui->listButtonsWidget->setMovement(QListView::Static); //not to move items with the mouse
    //temp
    ui->listButtonsWidget->setMinimumWidth(360);
    ui->listButtonsWidget->setMaximumHeight(85);
    ui->listButtonsWidget->setMinimumHeight(84); // precakva mi layouta
    // TEMP comment ui->listButtonsWidget->setFrameStyle(QFrame::Sunken); //hides frame

    //create Items for QListWidget
    new QListWidgetItem(QIcon(":/images/images/user.png"),tr("Account"),ui->listButtonsWidget); //index 0
    new QListWidgetItem(QIcon(":/images/images/user.png"),tr("Account"),ui->listButtonsWidget); //index 1
    new QListWidgetItem(QIcon(":/images/images/shares.png"),tr("Shares"),ui->listButtonsWidget); //index 2
    new QListWidgetItem(QIcon(":/images/images/shares.png"),tr("Sync"),ui->listButtonsWidget); //Sync Page index 3
    new QListWidgetItem(QIcon(":/images/images/settings.png"),tr("Settings"),ui->listButtonsWidget); //index 4
    new QListWidgetItem(QIcon(":/images/images/help.png"),tr("Help"),ui->listButtonsWidget); //index 5
    new QListWidgetItem(QIcon(":/images/images/info.png"),tr("About"),ui->listButtonsWidget); //index 6

    fillAcountNotLoggedPage();
    fillAboutPage();
    fillHelpPage();
    //p settngsPage = new SettingsPage(this, app);

    // indexes of Items in listWidget and their coresponding pages in StackWidget are the same
    connect(ui->listButtonsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

    //p connect(ui->btnShareFolder,SIGNAL(clicked()), app, SLOT(shareFolder()));

    //for resize
    for(int i = 0; i < ui->pagesWidget->count(); i++)
        ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    ui->verticalLayout_10->setAlignment(Qt::AlignCenter); //tab help

    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud");
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
        ui->listButtonsWidget->setRowHidden(4, true); // pcloud Settings;  for fs -> false
    }
    else
    {
        ui->listButtonsWidget->setRowHidden(0,false);
        ui->listButtonsWidget->setRowHidden(1,true);
        ui->listButtonsWidget->setRowHidden(2,true);
        ui->listButtonsWidget->setRowHidden(3,true);
        ui->listButtonsWidget->setRowHidden(4, true); // for fs -> false
    }
}

void PCloudWindow::setOnlinePages()
{
    //p sharesPage = new SharesPage(this, app);
    syncPage = new SyncPage(this, app);
    fillAccountLoggedPage();
}
void PCloudWindow::fillAcountNotLoggedPage()
{
    connect(ui->btnLogin, SIGNAL(clicked()),app, SLOT(showLogin()));
    connect(ui->btnRegstr, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->btnExit, SIGNAL(clicked()), app, SLOT(doExit())); // to move slot in this class
    connect(ui->toolBtnContact, SIGNAL(clicked()), this, SLOT(contactUs()));
    ui->toolBtnContact->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
}

void PCloudWindow::fillAboutPage()
{
    ui->label_versionVal->setText(APP_VERSION);
}

void PCloudWindow::fillHelpPage()
{
    connect(ui->tBtnOnlineHelp, SIGNAL(clicked()), this, SLOT(openOnlineHelp()));
    connect(ui->tBtnOnlineTutorial, SIGNAL(clicked()), this, SLOT(openOnlineTutorial()));
    connect(ui->tBtnFeedback, SIGNAL(clicked()), this, SLOT(sendFeedback()));
}

void PCloudWindow::fillAccountLoggedPage()
{
    ui->label_email->setText(app->username);
    if (app->isVerified)
    {
        ui->btnVerify->setVisible(false);
        ui->checkBoxVerified->setCheckState(Qt::Checked);
    }
    else
    {
        ui->checkBoxVerified->setCheckState(Qt::Unchecked);
        ui->btnVerify->setVisible(true);
        connect(ui->btnVerify, SIGNAL(clicked()), this, SLOT(verifyEmail()));
    }
    ui->checkBoxVerified->setEnabled(false);

    ui->progressBar_space->setMinimum(0);
    ui->progressBar_space->setMaximum(100);
    ui->progressBar_space->setValue(app->freeSpacePercentage);
    ui->progressBar_space->setFormat("%v% free");
    ui->label_space->setText(QString::number(app->usedSpace, 'f', 1) + " GB used of " + app->planStr);

    ui->label_planVal->setText(app->planStr);

    ui->toolBtnChangePass->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    connect(ui->toolBtnOpenWeb, SIGNAL(clicked()), this, SLOT(openWebPage()));
    // connect(ui->toolBtnChangePass, SIGNAL(clicked()), this, SLOT(changePass()));
    connect(ui->tbtnGetSpace, SIGNAL(clicked()), this, SLOT(upgradePlan()));
    connect(ui->label, SIGNAL(linkActivated(QString)), this, SLOT(upgradePlan()));
    ui->tbtnGetSpace->setVisible(false);
   //p connect(ui->tbtnOpenFolder, SIGNAL(clicked()),app,SLOT(openCloudDir()));
    connect(ui->tBtnExit, SIGNAL(clicked()), app, SLOT(doExit())); // to move in this class
    connect(ui->tBtnLogout, SIGNAL(clicked()), app, SLOT(logOut()));



    connect(ui->btnChangePass, SIGNAL(clicked()), this, SLOT(changePass()));
    connect(ui->btnUnlink, SIGNAL(clicked()), this, SLOT(unlinkSync()));

    //for sync hide some widgets till start use new fs
    ui->tBtnExit->setVisible(false);
    ui->tBtnLogout->setVisible(false);
    ui->toolBtnChangePass->setVisible(false);
    ui->line_2->setVisible(false);
    ui->toolBtnOpenWeb->setVisible(false);
    ui->tbtnOpenFolder->setVisible(false);
}

SyncPage* PCloudWindow::get_sync_page()
{
    return this->syncPage;
}

//SLOTS
void PCloudWindow::changePass() // hasn't implemented
{
    //psync_change_pass...
    ChangePassDialog *dialog = new ChangePassDialog();
    dialog->exec();
}
void PCloudWindow::unlinkSync()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,"", tr("If You unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        emit app->logOut(); //sets offlineimtes too
        emit setOnlineItems(false);
        // if (app->isLogedIn())
        app->setFirstLaunch(true);
        psync_unlink();
    }
}

void PCloudWindow::verifyEmail(){    // not implemented
    auth=app->authentication.toUtf8();
            //psync_verify_email(app->username); // check return val
    // psync_verify...
    QMessageBox::information(this, "Please check your e-mail", "E-mail verification sent to: "+app->username);
    verifyClicked = true;
}

void PCloudWindow::checkVerify() // has the user verified after had clicked "Verify Now"
{
    bool verified = psync_get_bool_value("emailverified");
    if (verified)
    {
        ui->checkBoxVerified->setChecked(true);
        ui->btnVerify->setVisible(false);
        verifyClicked = false;
    }
}

void PCloudWindow::openWebPage()
{
    QUrl url("https://my.pcloud.com/#page=filemanager");
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
    QUrl url ("https://my.pcloud.com/#page=faq"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::contactUs(){
    QUrl url ("https://my.pcloud.com/#page=contact");
    QDesktopServices::openUrl(url);
}


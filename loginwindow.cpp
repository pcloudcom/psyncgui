#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "psynclib.h"
#include <unistd.h>
#include <QDebug>

LoginWindow::LoginWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    app = a;
    ui->setupUi(this);
    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Drive");  
    ui->forgotPassBtn->setStyleSheet("QToolButton{background-color:transparent; text-decoration: underline;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    ui->tbtnReg->setStyleSheet("QToolButton{background-color:transparent; text-decoration: underline;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");    
    ui->loginButton->setDefault(true);
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(logIn()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(logIn()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->tbtnReg, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->forgotPassBtn,SIGNAL(clicked()), this,SLOT(forgotPassword()));
    connect(ui->btnUnlink, SIGNAL(clicked()), this, SLOT(unlinkSync()));
#ifndef Q_OS_WIN
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding); // to have min size for diff OSs
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
#else
    this->setFixedSize(570,412);
#endif     
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::showEvent(QShowEvent *)
{
    username = psync_get_username();
   // ui->error->setText("");
    if (username == "")
    {
        ui->btnUnlink->setVisible(false);
        ui->email->setEnabled(true);
        ui->email->clear();
        ui->password->clear();
        ui->email->setFocus();
    }
    else
    {
        ui->email->setText(username);
        ui->email->setEnabled(false);
        ui->btnUnlink->setVisible(true);
    }
}

void LoginWindow::focusPass(){
    ui->password->setFocus();
}

void LoginWindow::showError(const char *err)
{
    //ui->error->setText(trUtf8(err)); //for translation
    QMessageBox::critical(this, "pCloud Drive", trUtf8(err));
}

void LoginWindow::logIn()
{
   // ui->error->setText("");
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    int ischecked = ui->checkBox->isChecked()? 1: 0;
    QApplication::setOverrideCursor(Qt::WaitCursor); // or to add QSplashSCreen instead

    bool savedauth = psync_get_bool_value("saveauth"); //works when syns is paused also
    if (!savedauth)
        psync_set_user_pass(email,password,ischecked);
    else
        psync_set_pass(password,ischecked);

    QList<quint32> loginStatusLst;
    loginStatusLst << PSTATUS_BAD_LOGIN_DATA << PSTATUS_LOGIN_REQUIRED<< PSTATUS_BAD_LOGIN_TOKEN <<PSTATUS_USER_MISMATCH << PSTATUS_OFFLINE;

    int times = 0;
    for(;;)
    {
        times++;
        pstatus_t status;
        memset(&status, 0, sizeof(status));
        psync_get_status(&status);
        qDebug()<<"login btn "<< status.status<< times;
        if (status.status == PSTATUS_CONNECTING || status.status == PSTATUS_SCANNING )
        {
            sleep(1);
            continue;
        }
        else
        {
            if ((loginStatusLst.contains(status.status)))
            {
                if (status.status != PSTATUS_OFFLINE)
                {
                    this->showError("Invalid user and password combination");
                    psync_set_bool_setting("saveauth",false);
                }
                else
                    this->showError("No internet connection");
                QApplication::restoreOverrideCursor();
                return;
            }
            else
            {
                // PSTATUS_PAUSED is returned before and after PSTATUS_BAD_LOGIN_DATA
                if (status.status == PSTATUS_PAUSED && times == 1)
                {
                    sleep(3);
                    continue;
                }
                else
                    break;
            }
        }
    }

    app->logIn(email, ischecked); // thisway quoa won't be 0
    QApplication::restoreOverrideCursor();
    ui->password->clear();    
    this->close();
    //p app->openCloudDir();

}

void LoginWindow::forgotPassword()
{
    QByteArray email=ui->email->text().toUtf8();
    if (email.size() == 0){
        showError("Enter your email.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    char *err = NULL;
    int res = psync_lost_password(email,&err);
    if (!res) // returns 0 on success
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, trUtf8("Reset password"), trUtf8("An email with passowrd reset instructions is sent to your address."));
    }
    else
    {
        if (res == -1)
        {
            showError("No internet connection");
            QApplication::restoreOverrideCursor();
            return;
        }
        else
        {
             showError(err);
            QApplication::restoreOverrideCursor();
            return;
        }
    }
    QApplication::restoreOverrideCursor();
    free(err);    
}
void LoginWindow::unlinkSync() // to be moved in sync class
{    
    QMessageBox::StandardButton reply;
    //reply = QMessageBox::warning(this,trUtf8("Unlink"), trUtf8("If you unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
    reply = QMessageBox::warning(this,trUtf8("Unlink"), trUtf8("By unlinking you will lose all pCloud Drive settings for the current account on this computer. Are you sure?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        ui->email->setText("");
        ui->email->setEnabled(true);
        ui->email->setFocus();
        ui->btnUnlink->setVisible(false); 
        app->unlink();
    }
}

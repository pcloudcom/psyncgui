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
    app=a;
    ui->setupUi(this);
    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud");
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->error->setPalette(palette);
    ui->forgotPassBtn->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(logIn()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(logIn()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->registerButton, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->forgotPassBtn,SIGNAL(clicked()), this,SLOT(forgotPassword()));

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
void LoginWindow::showEvent(QShowEvent *)
{
    username = psync_get_username();
    ui->error->setText("");
    if (username == "")
    {
        ui->btnUnlink->setVisible(false);
        ui->email->setEnabled(true);
        ui->email->clear();
        ui->password->clear();
    }
    else
    {
        ui->email->setText(username);
        ui->email->setEnabled(false);
        ui->btnUnlink->setVisible(true);
        connect(ui->btnUnlink, SIGNAL(clicked()), this, SLOT(unlinkSync()));
    }
}

void LoginWindow::focusPass(){
    ui->password->setFocus();
}

void LoginWindow::setError(const char *err)
{
    ui->error->setText(trUtf8(err)); //for translation
}

void LoginWindow::logIn()
{
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    int ischecked = ui->checkBox->isChecked()? 1: 0;
    QApplication::setOverrideCursor(Qt::WaitCursor); // or to add QSplashSCreen instead
    //QString auth = psync_get_auth_string();
    //if (auth == "")
    bool savedauth = psync_get_bool_value("saveauth"); //works when syns is paused also
    if (!savedauth)
        psync_set_user_pass(email,password,ischecked);
    else
        psync_set_pass(password,ischecked);

    QList<quint32> loginStatusLst;
    loginStatusLst << PSTATUS_BAD_LOGIN_DATA << PSTATUS_LOGIN_REQUIRED <<PSTATUS_USER_MISMATCH << PSTATUS_OFFLINE;

    int times = 0;
    for(;;)
    {
        times++;
        pstatus_t status;
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
                this->setError("Invalid user and password combination");
                QApplication::restoreOverrideCursor();
                return;
            }
            else
            {
                // PSTATUS_PAUSED is returned before and after PSTATUS_BAD_LOGIN_DATA
                //if ((status.status == PSTATUS_PAUSED  || status.status == PSTATUS_READY) && times == 1)
                if (status.status == PSTATUS_PAUSED && times == 1)
                {
                    sleep(3);
                    //pstatus_t newstatus;
                    //psync_get_status(&newstatus);
                    //qDebug()<< newstatus.status;
                    //if(newstatus.status == PSTATUS_PAUSED)
                    //break;
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
    setError("");
    hide();
    //p app->openCloudDir();

}

void LoginWindow::forgotPassword()
{
    QByteArray email=ui->email->text().toUtf8();
    if (email.size() == 0){
        setError("Enter your email.");
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
            setError("No internet connection");
            QApplication::restoreOverrideCursor();
            return;
        }
        else
        {
            setError(err);
            QApplication::restoreOverrideCursor();
            return;
        }
    }
    QApplication::restoreOverrideCursor();
    free(err);
    setError("");
}
void LoginWindow::unlinkSync() // to be moved in sync class
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,"", trUtf8("If You unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        ui->email->setText("");
        ui->email->setEnabled(true);
        ui->email->setFocus();
        ui->btnUnlink->setVisible(false);
        app->setFirstLaunch(true);
        psync_unlink();
    }
}

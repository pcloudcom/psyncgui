#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "psynclib.h"
#include <QDebug>

LoginWindow::LoginWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    app=a;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
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
    if (username == "")
    {
        ui->btnUnlink->setVisible(false);
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
    ui->error->setText(err);
}

void LoginWindow::logIn()
{
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    int ischecked = ui->checkBox->isChecked()? 1: 0;
    if (psync_get_bool_value("saveauth"))
        psync_set_pass(password,ischecked);
        // iconkata
    else
        psync_set_user_pass(email,password,ischecked);
    app->logIn(email, ischecked);
    ui->password->clear();
    hide();
    //p app->openCloudDir();

    // QApplication::restoreOverrideCursor();
}

void LoginWindow::forgotPassword()
{
    QByteArray email=ui->email->text().toUtf8();

    if (email.size() == 0){
        setError("Enter your email.");
        return;
    }


}
void LoginWindow::unlinkSync() // to be moved in sync class
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,"", tr("If You unlink your account from this computer any data about your synced folders will be lost. Do you still want to unlink?"),
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

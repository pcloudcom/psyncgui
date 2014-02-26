#include "registerwindow.h"
#include "ui_registerwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "psynclib.h"

RegisterWindow::RegisterWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegisterWindow)
{
    app=a;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    connect(ui->registerButton, SIGNAL(clicked()), this, SLOT(doRegister()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(focusConfirm()));
    connect(ui->confirmpassword, SIGNAL(returnPressed()), this, SLOT(focusTOS()));
    connect(ui->loginButton, SIGNAL(clicked()), app, SLOT(showLogin()));
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void RegisterWindow::setError(const char *err){
    ui->error->setText(err);
}

void RegisterWindow::focusPass(){
    ui->password->setFocus();
}

void RegisterWindow::focusConfirm(){
    ui->confirmpassword->setFocus();
}

void RegisterWindow::focusTOS(){
    ui->accepttos->setFocus();
}

void RegisterWindow::doRegister(){
    if (!ui->accepttos->checkState()){
        setError("Registration only possible upon acceptance of terms.");
        return;
    }
    if (ui->password->text().length()<6){
        setError("Password too short - minimum 6 characters.");
        return;
    }
    if (ui->password->text()!=ui->confirmpassword->text()){
        setError("Password confirmation does not match.");
        return;
    }
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    int res = psync_register(email,password,true,NULL);
    if (!res)
        app->logIn(email,false);
    else{
        if (res == -1 )
        {
            setError("No internet connection");
            return;
        }
        //else ... err str
    }
    ui->password->clear();
    ui->confirmpassword->clear();
    hide();
    //p app->openCloudDir();

}

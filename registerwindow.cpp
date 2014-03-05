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
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->error->setPalette(palette);
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
void RegisterWindow::showEvent(QShowEvent *event)
{
    QString user = psync_get_username();
    if (user != "")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("pCloud");
        msgBox.setText(trUtf8 ("User %1 has already linked in.").arg(user));
        msgBox.setInformativeText(trUtf8 ("Do you want to unlink %1 and to continue").arg(user));
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        QAbstractButton *btnUnlink = msgBox.addButton(trUtf8("Unlink"), QMessageBox::YesRole);
        msgBox.exec();
        if (msgBox.clickedButton() == btnUnlink)
        {
            psync_unlink();
            app->setFirstLaunch(true); // to show suggestions list
            event->accept();
        }
        else
        {
            event->ignore();
            this->hide();
        }
    }
}

void RegisterWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void RegisterWindow::setError(const char *err){
    ui->error->setText(trUtf8(err));
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
    char *err = NULL;
    int res = psync_register(email,password,1,&err);
    if (!res)
    {
        psync_set_user_pass(email,password,0); //PSTATUS_LOGIN_REQUIRED appers after register
        app->logIn(email,false);
    }
    else
    {
        if (res == -1 )
        {
            setError("No internet connection");
            return;
        }
        else
        {
            setError(err);
            return;
        }
    }
    free(err);
    ui->password->clear();
    ui->confirmpassword->clear();
    setError("");
    hide();
    //p app->openCloudDir();

}

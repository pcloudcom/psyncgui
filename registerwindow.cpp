#include "registerwindow.h"
#include "ui_registerwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "psynclib.h"

RegisterWindow::RegisterWindow(PCloudApp *a, int pageIndex, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::RegisterWindow)
{
    app = a;
    ui->setupUi(this);
    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Drive");
    ui->stackedWidget->setCurrentIndex(pageIndex);
    if(pageIndex)
        this->setUnlinkLabelText();
    ui->registerButton->setDefault(true);
    ui->btnUnlink->setDefault(true);
    ui->tbtnLogin->setStyleSheet("QToolButton{background-color:transparent; text-decoration: underline;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    QFont italicFont;
    italicFont.setItalic(true);
    ui->label_accntinfo->setFont(italicFont);
    connect(ui->registerButton, SIGNAL(clicked()), this, SLOT(doRegister()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(focusConfirm()));
    connect(ui->confirmpassword, SIGNAL(returnPressed()), this, SLOT(focusTOS()));
    connect(ui->tbtnLogin, SIGNAL(clicked()), app, SLOT(showLogin()));
    connect(ui->btnUnlink, SIGNAL(clicked()), this, SLOT(askUnlink()));
    connect(ui->btnCancel, SIGNAL(clicked()), app, SLOT(showLogin()));
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding); // to have min size for diff OSs
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void RegisterWindow::showEvent(QShowEvent *)
{
    ui->accepttos->setCheckState(Qt::Unchecked);
    ui->email->clear();
    ui->confirmpassword->clear();
    ui->password->clear();
}

void RegisterWindow::setCurrPage(int index)
{
    if(index)
        this->setUnlinkLabelText();
    ui->stackedWidget->setCurrentIndex(index);
}

void RegisterWindow::setUnlinkLabelText()
{
    QString user = psync_get_username();
    if(user.length() > 30)
        user = user.left(30).append("...");
    ui->label_accntinfo->setText(user);    
}

void RegisterWindow::setError(const char *err){
    // ui->error->setText(trUtf8(err));
    QMessageBox::critical(this, "pCloud Drive", trUtf8(err));
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
    // setError("");
    hide();
    //p app->openCloudDir();

}

void RegisterWindow::askUnlink()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,trUtf8("Unlink"),
                                 trUtf8("By unlinking you will lose all pCloud Drive settings for the current account on this computer. Are you sure?"),
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        app->unlink();
        ui->stackedWidget->setCurrentIndex(0);
    }
}

#include <QMessageBox>
#include "sharefolderwindow.h"
#include "ui_sharefolderwindow.h"
#include "pcloudwindow.h"

ShareFolderWindow::ShareFolderWindow(PCloudWindow *w,SharesPage *sp, QWidget *parent) :    
    QMainWindow(parent),
    ui(new Ui::ShareFolderWindow)
{    
    sharePage = sp;
    pclwin = w;
    remoteFldrsDialog = new RemoteTreesDialog(pclwin);
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    connect(ui->cancelbutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->sharebutton, SIGNAL(clicked()), this, SLOT(shareFolder()));
    connect(ui->btnOpenRemoteDialog, SIGNAL(clicked()),remoteFldrsDialog,SLOT(exec()));
    connect(remoteFldrsDialog, SIGNAL(accepted()), this, SLOT(setFlrd()));

    this->setFixedSize(this->width(),this->height()); //makes the win not resizable

}

ShareFolderWindow::~ShareFolderWindow()
{
    delete ui;
    delete remoteFldrsDialog;
}

void ShareFolderWindow::showEvent(QShowEvent *event)
{   
    ui->email->clear();
    ui->editline_sharename->clear();
    ui->btnOpenRemoteDialog->setText(trUtf8("Choose remote folder"));
    ui->permCreate->setChecked(false);
    ui->permModify->setChecked(false);
    ui->permDelete->setChecked(false);
    ui->text_msg->clear();
    remoteFldrsDialog->init();
    event->accept();
}

void ShareFolderWindow::closeEvent(QCloseEvent *event)
{        
    hide();
    event->ignore();
}

//slots
void ShareFolderWindow::setFlrd()
{
    this->fldrid = remoteFldrsDialog->getFldrid();
    QString path = remoteFldrsDialog->getFldrPath(), fldrname;
    fldrname = path.section("/",-1);
    ui->editline_sharename->setText(fldrname);

    if(path.length() < 40)
        ui->btnOpenRemoteDialog->setText(path);
    else if(fldrname.length() < 40 )
        ui->btnOpenRemoteDialog->setText(QString("/.../" + fldrname));
    else
        ui->btnOpenRemoteDialog->setText(QString("/.../" + fldrname.right(40)));

}

static bool isValidEmail(const char* email){
    QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailREX.setCaseSensitivity(Qt::CaseInsensitive);
    mailREX.setPatternSyntax(QRegExp::RegExp);
    return mailREX.exactMatch(email);
}

void ShareFolderWindow::shareFolder()
{       
    if (ui->email->text().isEmpty())
    {
        showError("No email is specified.");
        return;
    }
    remoteFldrsDialog->getFldrPath();
    QStringList mails = ui->email->text().split(",");
    QByteArray name = ui->editline_sharename->text().toUtf8(), msg = ui->text_msg->toPlainText().toUtf8();
    quint64 perms = 1 + (ui->permCreate->isChecked()? PSYNC_PERM_CREATE:0)+
            (ui->permModify->isChecked()? PSYNC_PERM_MODIFY :0)+
            (ui->permDelete->isChecked()? PSYNC_PERM_DELETE :0);

    while (!mails.empty())
    {
        QByteArray mail=mails[0].trimmed().toUtf8();
        ui->email->setText(mails.join(","));
        if (!isValidEmail(mail))
        {
            char errmsg[512];
            strcpy(errmsg,mail);
            strcat(errmsg," is not a valid e-mail address.");
            this->showError(errmsg);
            return;
        }
        char* err = NULL;
        int res= psync_share_folder(fldrid,name,mail,msg,perms,&err);
        if(!res)
            mails.removeFirst();
        else
        {
            if(res != -1)
                this->showError(err);
            else
                this->showError("No internet connection");
            return;
        }
        free(err);
    }

    ui->email->clear();
    sharePage->refreshTab(0);
    this->hide();
}

void ShareFolderWindow::showError(const char* err)
{
    QMessageBox::critical(this,trUtf8("pCloud"), trUtf8(err));
}

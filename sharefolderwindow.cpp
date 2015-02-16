#include <QMessageBox>
#include "sharefolderwindow.h"
#include "ui_sharefolderwindow.h"
#include "pcloudwindow.h"

ShareFolderWindow::ShareFolderWindow(PCloudApp *a, PCloudWindow *w, QString path, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShareFolderWindow)
{
    ui->setupUi(this);
    app = a;
    pclwin = w;
    ui->rbtnView->setChecked(true);
    ui->rbtnManage->setChecked(false);
    ui->rbtnView->setChecked(false);

    remoteFldrsDialog = new RemoteTreesDialog("", this);

    if(path == NULL)
        this->contxMenuFlag = false;
    else
    {
        qDebug()<<"ShareFolderWindow "<<path;
        this->contxMenuFlag = true;
        this->fldrPath = path;
    }
    setWindowIcon(QIcon(WINDOW_ICON));
    connect(ui->btnOpenRemoteDialog, SIGNAL(clicked()),remoteFldrsDialog, SLOT(exec()));
    connect(remoteFldrsDialog, SIGNAL(accepted()), this, SLOT(setFlrd()));
    connect(ui->cancelbutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->sharebutton, SIGNAL(clicked()), this, SLOT(shareFolder()));

    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
}

ShareFolderWindow::~ShareFolderWindow()
{
    delete ui;
    delete remoteFldrsDialog;
}

void ShareFolderWindow::showEvent(QShowEvent *event)
{
    qDebug()<<"ShareFolderWindow::showEvent";
    ui->email->clear();
    if(!contxMenuFlag)
    {
        fldrid = -1;
        ui->editline_sharename->clear();
        ui->btnOpenRemoteDialog->setVisible(true);
        ui->btnOpenRemoteDialog->setText(trUtf8("Choose pCloud Drive Folder"));
        remoteFldrsDialog->init();
    }
    else
    {
        this->displayShareName();
        ui->btnOpenRemoteDialog->setVisible(false);
    }

    ui->rbtnView->setChecked(true);
    ui->text_msg->clear();

    if(psync_get_bool_value("business"))
        ui->rbtnManage->setVisible(true);
    else
        ui->rbtnManage->setVisible(false);

    event->accept();
}

void ShareFolderWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void ShareFolderWindow::setContextMenuFlag(bool flag)
{
    this->contxMenuFlag = flag;
}

void ShareFolderWindow::setFldrbyMenu(QString path)
{
    this->fldrPath = path;
}

void ShareFolderWindow::displayShareName()
{
    qDebug() <<"ShareFolderWindow::displayShareName"<<this->fldrPath;
    QString fldrname =  this->fldrPath.section("/",-1);
    if(fldrname.length() < 40)
        ui->editline_sharename->setText(fldrname);
    else
        ui->editline_sharename->setText(QString(fldrname.left(40) + "..."));

    pentry_t *pfldr = psync_stat_path(this->fldrPath.toUtf8());
    fldrid = pfldr->folder.folderid;
}

//slots
void ShareFolderWindow::setFlrd()
{
    this->fldrid = remoteFldrsDialog->getFldrid();
    QString  path = remoteFldrsDialog->getFldrPath(), fldrname;
    fldrname = path.section("/",-1);
    ui->editline_sharename->setText(fldrname);
    if(path.length() < 40)
        ui->btnOpenRemoteDialog->setText(path);
    else if(fldrname.length() < 40 )
        ui->btnOpenRemoteDialog->setText(QString("/.../" + fldrname));
    else
        ui->btnOpenRemoteDialog->setText(QString("/.../" + fldrname.left(40) + "..."));
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

    if (!app->isVerified)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(pclwin,"Your Account is not Verified", "This functionality is available for verified accounts only.\nDo you want to verify your account now?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            pclwin->verifyEmail();
        return;
    }

    QStringList mails = ui->email->text().remove(" ").split(",");
    mails.removeDuplicates();

    QByteArray name = ui->editline_sharename->text().toUtf8(), msg = ui->text_msg->toPlainText().toUtf8();
    quint64 perms = 1 + (ui->rbtnEdit->isChecked()? PSYNC_PERM_CREATE + PSYNC_PERM_MODIFY + PSYNC_PERM_DELETE :0); //+
             // (ui->rbtnManage->isChecked()? MANAGE_FLAG :0); // TO DO

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
        int res = psync_share_folder(fldrid,name,mail,msg,perms,&err);
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
    emit this->pclwin->refreshPageSlot(SHARES_PAGE_NUM,0);
    this->hide();
}

void ShareFolderWindow::showError(const char* err)
{
    QMessageBox::critical(this,trUtf8("pCloud"), trUtf8(err));
}

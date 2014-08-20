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
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    connect(ui->cancelbutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->sharebutton, SIGNAL(clicked()), this, SLOT(shareFolder()));
    connect(ui->dirtree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(dirSelected(QTreeWidgetItem *)));

}

ShareFolderWindow::~ShareFolderWindow()
{
    delete ui;
}

void ShareFolderWindow::showEvent(QShowEvent *event)
{
    pclwin->initRemoteTree(ui->dirtree);
    ui->dirtree->setCurrentItem(NULL);
    ui->sharename->clear();
    event->accept();
}

void ShareFolderWindow::closeEvent(QCloseEvent *event)
{        
    hide();
    event->ignore();
}


//slots
void ShareFolderWindow::dirSelected(QTreeWidgetItem *dir)
{
    if (dir)
        ui->sharename->setText(dir->text(0));
}

static bool isValidEmail(const char* email){
    QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailREX.setCaseSensitivity(Qt::CaseInsensitive);
    mailREX.setPatternSyntax(QRegExp::RegExp);
    return mailREX.exactMatch(email);
}

void ShareFolderWindow::shareFolder()
{
    if (!ui->dirtree->currentItem())
    {
        showError("No folder is selected.");
        return;
    }

    if (ui->email->text().isEmpty())
    {
        showError("No email is specified.");
        return;
    }

    QStringList mails = ui->email->text().split(","); //esil_duran_psync@abv.bg
    QByteArray name = ui->sharename->text().toUtf8(), msg = ui->text_msg->toPlainText().toUtf8();
    quint64 folderid = ui->dirtree->currentItem()->data(1, Qt::UserRole).toULongLong();
    quint64 perms = (ui->permCreate->isChecked()? PSYNC_PERM_CREATE:0)+
            (ui->permModify->isChecked()? PSYNC_PERM_MODIFY :0)+
            (ui->permDelete->isChecked()? PSYNC_PERM_DELETE :0);

    while (!mails.empty())
    {
        QByteArray mail=mails[0].trimmed().toUtf8();
        ui->email->setText(mails.join(","));
        if (!isValidEmail(mail))
        {
            char errmsg[256];
            strcpy(errmsg,mail);
            strcat(errmsg," is not a valid e-mail address.");
            this->showError(errmsg);
            return;
        }
        char* err = NULL;
        // psync_share_folder(psync_folderid_t folderid, const char *name, const char *mail, const char *message, uint32_t permissions, char **err);
        int res= psync_share_folder(folderid,name,mail,msg,perms,&err);
        if(!res)
            mails.removeFirst();
        else
        {
            if(res != -1)
                this->showError(err);
            else
                this->showError("No internet connection");
            break;
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

/*
void ShareFolderWindow::showEvent(QShowEvent *)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    ui->dirtree->clear();
    ui->dirtree->setColumnCount(1);
    ui->dirtree->setHeaderLabels(QStringList("Name"));
    res=send_command(conn, "listfolder",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_STR("filtermeta", "contents,folderid,name"),
                     P_NUM("folderid", 0),
                     P_BOOL("recursive", 1),
                     P_BOOL("nofiles", 1),
                     P_BOOL("noshares", 1));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError("Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(find_res(res, "error")->str);
        free(res);
        return;
    }
    result=find_res(find_res(res, "metadata"), "contents");
    ui->dirtree->insertTopLevelItems(0, binresToQList(result));

    free(res);
}

*/

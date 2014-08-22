#include "remotetreesdialog.h"
#include "ui_remotetreesdialog.h"
#include "pcloudwindow.h"

RemoteTreesDialog::RemoteTreesDialog(PCloudWindow* &w,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteTreesDialog)
{
    ui->setupUi(this);
    ui->widget_fldrName->setVisible(false); /// for acceptshare - to edin fldr name
    w->initRemoteTree(ui->treeRemoteFldrs);
    root = ui->treeRemoteFldrs->currentItem();

    connect(ui->btnAccept, SIGNAL(clicked()), this,SLOT(setSelectedFolder()));
    connect(ui->btnReject, SIGNAL(clicked()),this,SLOT(hide()));

    this->setWindowIcon(QIcon(WINDOW_ICON));
    this->setWindowTitle("pCloud");

}

RemoteTreesDialog::~RemoteTreesDialog()
{
    delete ui;
}

void RemoteTreesDialog::showEvent(QShowEvent *event)
{
    ui->treeRemoteFldrs->setCurrentItem(this->root);
    event->accept();
}

void RemoteTreesDialog::setSelectedFolder()
{    
    if(!ui->treeRemoteFldrs->currentItem())
    {
        QMessageBox::warning(this,"pCloud",trUtf8("No remote folder is selected. Please click on a folder to select"));
        return;
    }
    else
    {
        fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
        fldrPath = ui->treeRemoteFldrs->currentItem()->data(0,Qt::UserRole).toString();
        this->accept();
    }
}

quint64 RemoteTreesDialog::getFldrid()
{
    return this->fldrid;
}
QString RemoteTreesDialog::getFldrPath()
{
    return this->fldrPath;
}

#include "remotetreesdialog.h"
#include "ui_remotetreesdialog.h"
#include "pcloudwindow.h"

RemoteTreesDialog::RemoteTreesDialog(PCloudWindow* &w,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteTreesDialog)
{
    ui->setupUi(this);      
    w->initRemoteTree(ui->treeRemoteFldrs);

    connect(ui->btnAccept, SIGNAL(clicked()), this,SLOT(setSelectedFolder()));
    connect(ui->btnReject, SIGNAL(clicked()),this,SLOT(hide()));

    this->setWindowIcon(QIcon(WINDOW_ICON));
    this->setWindowTitle("pCloud");

    //hide fldr name
}

RemoteTreesDialog::~RemoteTreesDialog()
{
    delete ui;
}

void RemoteTreesDialog::setSelectedFolder()
{
    fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
    this->accept();
}

quint64 RemoteTreesDialog::getFldrid()
{
    return this->fldrid;
}


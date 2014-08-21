#include "acceptsharedialog.h"

AcceptShareDialog::AcceptShareDialog(QString name,PCloudWindow* &w,QWidget *parent):
    RemoteTreesDialog(w,parent)
{        
    ui->line_folderName->setText(name);
    ui->btnAccept->setText("Accept");
    ui->btnReject->setText("Reject");
}

void AcceptShareDialog::setSelectedFolder()
{
    fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
    sharename = ui->line_folderName->text();
    this->accept();
}

QString AcceptShareDialog::getShareName()
{
    return this->sharename;
}

#include "acceptsharedialog.h"

AcceptShareDialog::AcceptShareDialog(QString fldrname, QWidget *parent):
    RemoteTreesDialog("",parent)
{        
    ui->widget_fldrName->setVisible(true);
    ui->line_folderName->setText(fldrname);
    ui->btnAccept->setText("Accept");    
}

void AcceptShareDialog::setSelectedFolder()
{
    if(!ui->treeRemoteFldrs->currentItem())
    {
        QMessageBox::warning(this,"pCloud",trUtf8("No remote folder is selected. Please click on a folder to select"));
        return;
    }
    fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
    sharename = ui->line_folderName->text();
    this->accept();
}

QString AcceptShareDialog::getShareName()
{
    return this->sharename;
}

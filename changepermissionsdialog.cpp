#include "changepermissionsdialog.h"
#include "ui_changepermissionsdialog.h"

ChangePermissionsDialog::ChangePermissionsDialog(quint32 perms, QString folderName, QString sharedWith, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePermissionsDialog)
{
    ui->setupUi(this);
    ui->foldername->setText(folderName);
    ui->sharedwith->setText(sharedWith);
    ui->permRead->setChecked(true);
    ui->permRead->setEnabled(false);
    ui->permModify->setChecked(perms & PSYNC_PERM_MODIFY);
    ui->permCreate->setChecked(perms & PSYNC_PERM_CREATE);
    ui->permDelete->setChecked(perms & PSYNC_PERM_DELETE);
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnSave,SIGNAL(clicked()),this,SLOT(setNewPermissions()));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

ChangePermissionsDialog::~ChangePermissionsDialog()
{
    delete ui;
}

void ChangePermissionsDialog::setNewPermissions()
{
    newPerms = 1 + (ui->permCreate->isChecked()? PSYNC_PERM_CREATE:0) +
            (ui->permModify->isChecked()? PSYNC_PERM_MODIFY :0) +
            (ui->permDelete->isChecked()? PSYNC_PERM_DELETE :0);

    emit this->accept();
}

quint32 ChangePermissionsDialog::getNewPermissions()
{
    return this->newPerms;
}

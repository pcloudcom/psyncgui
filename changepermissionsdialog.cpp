#include "changepermissionsdialog.h"
#include "ui_changepermissionsdialog.h"

ChangePermissionsDialog::ChangePermissionsDialog(quint32 perms, QString folderName, QString sharedWith, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePermissionsDialog)
{
    ui->setupUi(this);
    ui->foldername->setText(folderName);
    ui->sharedwith->setText(sharedWith);
    if (perms & PSYNC_PERM_WRITE)
        ui->rbtnEdit->setChecked(true);
    else
        ui->rbtnView->setChecked(true);

    //if(psync_get_bool_value("business") && !manageFlag)
    ui->rbtnManage->setVisible(false); //TO DO check isbusiness!

    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnSave,SIGNAL(clicked()),this,SLOT(setNewPermissions()));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding); // to have min size for diff OSs
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
}

ChangePermissionsDialog::~ChangePermissionsDialog()
{
    delete ui;
}

void ChangePermissionsDialog::setNewPermissions()
{
    newPerms = 1 + (ui->rbtnEdit->isChecked()? PSYNC_PERM_CREATE + PSYNC_PERM_MODIFY + PSYNC_PERM_DELETE :0); //+
    // (ui->rbtnManage->isChecked()? PSYNC_PERM_MANAGE :0); // TO DO

    emit this->accept();
}

quint32 ChangePermissionsDialog::getNewPermissions()
{
    return this->newPerms;
}

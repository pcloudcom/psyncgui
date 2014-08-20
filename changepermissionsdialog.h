#ifndef CHANGEPERMISSIONSDIALOG_H
#define CHANGEPERMISSIONSDIALOG_H

#include <QDialog>
#include "psynclib.h"

namespace Ui {
class ChangePermissionsDialog;
}

class ChangePermissionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChangePermissionsDialog(quint32 perms, QString folderName, QString sharedWith, QWidget *parent = 0);
    ~ChangePermissionsDialog();
    Ui::ChangePermissionsDialog *ui;
    quint32 getNewPermissions();
    quint32 newPerms;
public slots:    
    void setNewPermissions();
};

#endif // CHANGEPERMISSIONSDIALOG_H

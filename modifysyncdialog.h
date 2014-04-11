#ifndef MODIFYSYNCDIALOG_H
#define MODIFYSYNCDIALOG_H

#include <QDialog>
#include "syncpage.h"
#include "welcomescreen.h"

namespace Ui {
class ModifySyncDialog;
}

class SyncPage;
class WelcomeScreen;

class ModifySyncDialog : public QDialog
{
    Q_OBJECT

public:
    friend class SyncPage;    
    explicit ModifySyncDialog(QString local, QString remote, int type,QWidget *parent = 0);
    ~ModifySyncDialog();
    int returnNewType();
private:
    Ui::ModifySyncDialog *ui;
};

#endif // MODIFYSYNCDIALOG_H

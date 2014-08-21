#ifndef REMOTETREESDIALOG_H
#define REMOTETREESDIALOG_H

#include "psynclib.h"
#include "common.h"
#include <QDialog>

class PCloudWindow;

namespace Ui {
class RemoteTreesDialog;
}

class RemoteTreesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteTreesDialog(PCloudWindow* &w,QWidget *parent = 0);
    ~RemoteTreesDialog();
    quint64 getFldrid();       
protected:
    Ui::RemoteTreesDialog *ui;
    quint64 fldrid;
public slots:
    virtual void setSelectedFolder();
};

#endif // REMOTETREESDIALOG_H

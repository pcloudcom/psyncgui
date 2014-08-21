#ifndef ACCEPTSHAREDIALOG_H
#define ACCEPTSHAREDIALOG_H

#include "remotetreesdialog.h"
#include "ui_remotetreesdialog.h"
#include <QDialog>


class PCloudWindow;

class AcceptShareDialog : public RemoteTreesDialog
{
    Q_OBJECT
public:    
    AcceptShareDialog(QString name, PCloudWindow* &w, QWidget *parent = 0);
    QString getShareName();
private:
    QString sharename;
public slots:
    virtual void setSelectedFolder();
};

#endif // ACCEPTSHAREDIALOG_H

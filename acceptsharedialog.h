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
    AcceptShareDialog(QString fldrname, QWidget *parent);
    QString getShareName();
private:
    QString sharename;
public slots:
    virtual void setSelectedFolder();
};

#endif // ACCEPTSHAREDIALOG_H

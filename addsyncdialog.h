#ifndef ADDSYNCDIALOG_H
#define ADDSYNCDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <syncpage.h>
#include "pcloudapp.h"
#include "pcloudwindow.h"
#include "welcomescreen.h"


namespace Ui {
class addSyncDialog;
}
class SyncPage;
class WelcomeScreen;
class PCloudWindow;
class addSyncDialog : public QDialog
{
    Q_OBJECT

public:
    friend class WelcomeScreen;
    explicit addSyncDialog(PCloudApp *a,PCloudWindow *w,SyncPage *sp,WelcomeScreen *wlcm, QWidget *parent = 0);
    ~addSyncDialog();

private:
    Ui::addSyncDialog *ui;
    PCloudApp *app;
    PCloudWindow *win;
    SyncPage *syncpage;
    WelcomeScreen *welcomewin;
    QFileSystemModel *model;
    void showError(const QString &err);
    void load();
protected:
    void showEvent(QShowEvent *);
public slots:
    void addSync();
    void newLocalFldr();
    void newRemoteFldr();
    void hideDialog();
};

#endif // ADDSYNCDIALOG_H


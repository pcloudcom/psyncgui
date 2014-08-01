#ifndef ADDSYNCDIALOG_H
#define ADDSYNCDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <syncpage.h>
//#include "pcloudapp.h"
//#include "pcloudwindow.h"
#include "suggestnsbasewin.h"


namespace Ui {
class addSyncDialog;
}
class SyncPage;
class SuggestnsBaseWin;
class PCloudWindow;
class PCloudApp;
class addSyncDialog : public QDialog
{
    Q_OBJECT

public:
    friend class SuggestnsBaseWin;
    explicit addSyncDialog(PCloudApp *a,PCloudWindow *w,SyncPage *sp,SuggestnsBaseWin *wlcm, QWidget *parent = 0);     
    ~addSyncDialog();
private:
    Ui::addSyncDialog *ui;
    PCloudApp *app;
    PCloudWindow *win;
    SyncPage *syncpage;
    SuggestnsBaseWin *addNewSyncsWin;
    QFileSystemModel *model;
    void showError(const QString &err);
    void load();
public slots:
    void addSync();
    void newLocalFldr();
    void newRemoteFldr();
    void hideDialog();
};

#endif // ADDSYNCDIALOG_H


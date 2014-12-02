#ifndef ADDSYNCDIALOG_H
#define ADDSYNCDIALOG_H

#include <syncpage.h>
//#include "pcloudapp.h"
//#include "pcloudwindow.h"
#include "suggestnsbasewin.h"
#include "remotetreesdialog.h"
#include "common.h"
#include <QFileDialog>
#include <QDialog>
#include <QFileSystemModel>

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
    explicit addSyncDialog(PCloudApp *a,PCloudWindow *w,SuggestnsBaseWin *wlcm, QWidget *parent = 0);
    ~addSyncDialog();
private:
    Ui::addSyncDialog *ui;
    PCloudApp *app;
    PCloudWindow *win;
    SuggestnsBaseWin *addNewSyncsWin;
    //QFileSystemModel *model; // the old tree
    RemoteTreesDialog *remotesDialog;
    QString localpath, remotepath;
    QString getDisplFldrPath(QString fldrpath, QChar osSep);
    void showError(const QString &err);
    void load();
public slots:
    void chooseLocalFldr();
    void chooseRemoteFldr();
    void addSync();
    //  void newLocalFldr(); //obsolete
    void newRemoteFldr(QString dirname);
    void hideDialog();  
};

#endif // ADDSYNCDIALOG_H


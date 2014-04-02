#ifndef SYNCPAGE_H
#define SYNCPAGE_H
#include "pcloudapp.h"
#include "pcloudwindow.h"
#include "psynclib.h"
#include "addsyncdialog.h"

#include <QWidget>

class SyncPage : public QWidget // or qmainwin
{
    Q_OBJECT
public:
    friend class PCloudWindow;
    friend class addSyncDialog;
    explicit SyncPage(PCloudWindow *w, PCloudApp *a,QWidget *parent = 0);
    void load();
private:
    PCloudWindow *win;
    PCloudApp *app;
    bool SSL, p2p;
    QString minLocalSpace;
    int upldSpeed, upldSpeedNew;
    int dwnldSpeed, dwnldSpeedNew;
    QString patterns;
    void initSyncPage();
    void loadSettings();
    void showError();
public slots:
    void refreshTab(int index);
    void modifySync();
    void stopSync();    
    void addSync();
    void saveSettings();
    void cancelSettings();
    void enableSaveBtn();
    void setNewDwnldSpeed();
    void setNewUpldSpeed();
    void setNewSpeedFromEditline();
};

#endif // SYNCPAGE_H

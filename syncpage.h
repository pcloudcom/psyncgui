#ifndef SYNCPAGE_H
#define SYNCPAGE_H

#include "psynclib.h"
#include "addsyncdialog.h"
#include "settingspage.h"

#include <QWidget>
#include <QTreeWidgetItem>

class PCloudApp;
class PCloudWindow;

class SyncPage : public QWidget // or qmainwin
{
    Q_OBJECT
public:
    friend class PCloudWindow;
    friend class addSyncDialog;
    explicit SyncPage(PCloudWindow *w, PCloudApp *a,QWidget *parent = 0);
    void load();
    void loadSettings(); //prv
private:
    PCloudWindow *win;
    PCloudApp *app;
    QString patterns;
    void initSyncPage();
public slots:
    void openTab(int index);
    void refreshTab(int index);
    void syncDoubleClicked(QTreeWidgetItem *item,int col);
    //void modifySync();
    void stopSync(QTreeWidgetItem *item,int col);
    void addSync();
    void saveSettings();
    void cancelSettings();
    void enableSaveBtn();    
};

#endif // SYNCPAGE_H

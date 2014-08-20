#ifndef PCLOUDWINDOW_H
#define PCLOUDWINDOW_H
#include "sharespage.h"
#include "syncpage.h"
#include "psynclib.h"
#include "versiondwnldthread.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QMainWindow>

namespace Ui {
class PCloudWindow;
}

class PCloudApp;
class SettingsPage;
class SharesPage;
class SyncPage;

class PCloudWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class PCloudApp;
    friend class SettingsPage; // to access ui
    friend class SharesPage;    
    friend class SyncPage;    
    explicit PCloudWindow(PCloudApp *a, QWidget *parent = 0);
    ~PCloudWindow();
   //QList<QTreeWidgetItem *> listRemoteFldrs(QString parentPath);
    void initRemoteTree(QTreeWidget* table); // fill remote trees in differnt woins/dialogs
    SyncPage* get_sync_page();    
private:
    Ui::PCloudWindow *ui;
    PCloudApp *app;
    VersionDwnldThread *vrsnDwnldThread;
    SettingsPage *settngsPage;
    SharesPage *sharesPage;
    SyncPage *syncPage;
    QByteArray auth; // to del
    bool verifyClicked;
    void checkVerify();
    void closeEvent(QCloseEvent *event);
    void fillAcountNotLoggedPage();
    void fillAccountLoggedPage();    
    void fillAboutPage();
    void refreshPages();
protected:
    void showEvent(QShowEvent *);    
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void showpcloudWindow(int index);
    void setOnlineItems(bool online);
    void setOnlinePages();  //when the user logs in
    void refreshUserinfo();    
    void openMyPcloud();
    void shareFolder();
    void openOnlineTutorial();
    void openOnlineHelp();
    void sendFeedback();
    void contactUs();
    void updateVersion();
    void changePass();
    void forgotPass();
    void upgradePlan();
    void verifyEmail();
    void unlinkSync(); // to move in sync.h
};

#endif // PCLOUDWINDOW_H

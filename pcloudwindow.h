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
    int getCurrentPage();
    SyncPage* get_sync_page();    
    void refreshPagePulbic(int pageindex, int param);    
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
    void fillAccountLoggedPage();    
    void fillAboutPage();
    void refreshPage(int currentIndex);
    void setFrameProps(QFrame* frame);
protected:
    void showEvent(QShowEvent *);    
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    void refreshPageSgnl(int pageindex, int param);
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void setCurrntIndxPclWin(int index);    
    void setOnlineItems(bool online);   
    void refreshUserinfo();    
    void openMyPcloud();    
    void openOnlineTutorial();
    void openOnlineHelp();
    void sendFeedback();
    void contactUs();
    void updateVersion();
    void changePass();
    void forgotPass();
    void upgradePlan();
    void verifyEmail();
    void unlinkSync();
    void refreshPageSlot(int pageindex, int param);
};

#endif // PCLOUDWINDOW_H

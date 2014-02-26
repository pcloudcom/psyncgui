#ifndef PCLOUDWINDOW_H
#define PCLOUDWINDOW_H
//p in fs+sync only  #include "settingspage.h"
//p #include "sharespage.h"
//p #include "sharefolderwindow.h"
#include "syncpage.h"
#include "psynclib.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QMainWindow>

namespace Ui {
class PCloudWindow;
}

class PCloudApp;
//p class SettingsPage;
//p class SharesPage;
class SyncPage;

class PCloudWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class PCloudApp;
    //p friend class SettingsPage; // to access ui
    //p friend class SharesPage;
    //p friend class ShareFolderWindow;
    friend class SyncPage;
    explicit PCloudWindow(PCloudApp *a, QWidget *parent = 0);
    ~PCloudWindow();
    SyncPage* get_sync_page();

private:
    Ui::PCloudWindow *ui;
    PCloudApp *app;
    //p  SettingsPage *settngsPage;
    //p SharesPage *sharesPage;
    SyncPage *syncPage;
    QByteArray auth; // to del
    bool verifyClicked;
    void checkVerify();
    void closeEvent(QCloseEvent *event);
    void fillAcountNotLoggedPage();
    void fillAccountLoggedPage();
    void fillHelpPage();
    void fillAboutPage();
    void refreshPages();
protected:
    void showEvent(QShowEvent *);
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void showpcloudWindow(int index);
    void setOnlineItems(bool online);
    void setOnlinePages();  //when the user logs in
    void openWebPage();
    void openOnlineTutorial();
    void openOnlineHelp();
    void sendFeedback();
    void contactUs();
    void changePass();
    void upgradePlan();
    void verifyEmail();
    void unlinkSync(); // to move in sync.h
};

#endif // PCLOUDWINDOW_H

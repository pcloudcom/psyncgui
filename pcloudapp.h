#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "registerwindow.h"
#include "loginwindow.h"
//p #include "sharefolderwindow.h"
//p #include "shareswindow.h"
#include "psettings.h"
//#include "monitoringthread.h"
//#include "onlinethread.h"
//#include "revnotifythread.h"
#include "welcomescreen.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h" //temp
#include "psynclib.h"
#include <QApplication>
#include <QAction>
#include <QSystemTrayIcon>


class WelcomeScreen;

class PCloudApp : public QApplication
{
    Q_OBJECT
private:
    QAction *registerAction;
    QAction *loginAction;
    QAction *exitAction;
    QAction *logoutAction;
    //p  QAction *openAction; //open pcld fldr
    QAction *accountAction;
    //p QAction *settingsAction;  //Settings page
    //p QAction *sharesAction; // Shares page(2)
    QAction *syncAction; //Sync Tab (3)
    QAction *helpAction; // Help Tab (5)
    QAction *aboutPCloudAction; // About tab(6)
    QAction *pauseSyncAction;
    QAction *resumeSyncAction;
    QAction *addSyncAction;
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QMenu *syncMenu;
    QSystemTrayIcon *tray;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    //p ShareFolderWindow *sharefolderwin;
    //MonitoringThread *mthread;
    QListWidget *syncStatusListWidget;
    bool isFirstLaunch; //hardcoded for tests
#ifdef Q_OS_WIN
    RevNotifyThread *notifythread;
#endif
    // OnlineThread *othread;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
    void showWindow(QMainWindow *win);
    void createSyncFolderActions(QMenu *syncMenu);
public:
    static PCloudApp *appStatic;
    PCloudWindow *pCloudWin;
    WelcomeScreen *welcomeWin;
    QString username;
    QString authentication;
    bool rememberMe;
    bool isPremium;
    bool isVerified;
    QString planStr;
    quint64 freeSpacePercentage;
    qreal usedSpace;
    PSettings *settings;
    quint64 bytestoDwnld;
    quint64 bytestoUpld;
    quint32 filesToDwnld;
    quint32 filesToUpld;
    quint64 dwnldSpeed;
    quint64 upldSpeed;
    uint32_t lastMessageType; // for Shares; online status
    bool isCursorChanged;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    //p bool isMounted();
    //p void mount();
    //p void unMount();
    void logIn(QString uname, bool remember);
    //p void setSettings();
    void showError(QString err);
    void showOnClick();
    bool isLogedIn();
    void setLogWinError(const char *msg); // sets err in logwin; called from static function status_callback
    void changeSyncIconPublic(const QString icon); //called from static function status_callbac;  signals are protected and can't be accessed by static vars
    void changeCursorPublic(bool change);
    void setTextErrPublic(int win , const char *err);
    void addNewFolderInMenu(QAction *fldrAction); // refresh menu when add new sync
    QString bytesConvert(quint64 bytes);
    QString timeConvert(quint64 seconds);
    bool isMenuActive();
    void setFirstLaunch(bool b); // case after unlink - to display suggestions
signals:
    void changeSyncIcon(const QString icon);
    void changeCursor(bool change);
    void sendErrText(int win, const char *err);
public slots:
    // void showTrayMessage(QString title, QString msg);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showAccount();
    //p void showShares();
    void showSync();
    //p void showSettings();
    void showpCloudAbout();
    void showpcloudHelp();
    //p void openCloudDir();
    //p void shareFolder();
    void logOut();
    void doExit();
    //p void trayMsgClicked(); //show shares
    //p void setOnlineStatus(bool online);
    void setTrayIcon(const QString icon);
    void setCursor(bool change);
    void setErrText(int win, const char *err);
    void pauseSync();
    void resumeSync();
    void openLocalDir(); // for local sync folder
    void addNewSync();
    void updateSyncStatusInMenu();
};

#endif // PCLOUDAPP_H


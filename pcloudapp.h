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
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>


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
    QAction *syncDownldAction;
    QAction *syncUpldAction;
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QMenu *syncMenu;
    QSystemTrayIcon *tray;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    //p ShareFolderWindow *sharefolderwin;
    //MonitoringThread *mthread;
   // QListWidget *syncStatusListWidget; to del
    bool isFirstLaunch; //hardcoded for tests
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg;
    QNetworkSession *session;
    void getQuota();
    void getUserInfo();
#ifdef Q_OS_WIN
    //pRevNotifyThread *notifythread;
#endif
    // OnlineThread *othread;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
    void showWindow(QMainWindow *win);
public:
    static PCloudApp *appStatic;
    PCloudWindow *pCloudWin;
    WelcomeScreen *welcomeWin;
    QString username, authentication;
    QString downldInfo, uplodInfo;
    bool rememberMe;
    bool isPremium;
    bool isVerified;
    QString planStr;
    quint64 freeSpacePercentage;
    qreal usedSpace;
    PSettings *settings;
    bool upldFlag,downldFlag;
    quint64 bytestoDwnld;
    quint64 bytestoUpld;
    uint32_t lastMessageType; // for Shares; online status
    bool isCursorChanged;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    //p bool isMounted();
    //p void mount();
    //p void unMount();
    void logIn(const QString &uname, bool remember);
    //p void setSettings();
    void showError(QString &err);
    void showOnClick();
    bool isLogedIn();    
    void setLogWinError(const char *msg); // sets err in logwin; called from static function status_callback
    void changeSyncIconPublic(const QString &icon); //called from static function status_callbac;  signals are protected and can't be accessed by static vars
    void changeCursorPublic(bool change);
    void updateSyncStatusPublic();
    void updateUserInfoPublic(const char* param);
    void setTextErrPublic(int win , const char *err);
    void createSyncFolderActions(QMenu *syncMenu);
    QMenu* getSyncMenu();
    void addNewFolderInMenu(QAction *fldrAction); // refresh menu when add new sync
    QString bytesConvert(quint64 bytes);
    QString timeConvert(quint64 seconds);
    bool isMenuorWinActive();
    void setFirstLaunch(bool b); // case after unlink - to display suggestions
     void check_error();
signals:
    void changeSyncIcon(const QString &icon);
    void changeCursor(bool change);
    void sendErrText(int win, const char *err);
    void updateSyncStatusSgnl();
    void updateUserInfoSgnl(const char* &param);
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
    void setTrayIcon(const QString &icon);
    void setCursor(bool change);
    void setErrText(int win, const char *err);
    void pauseSync();
    void resumeSync();
    void openLocalDir(); // for local sync folder
    void addNewSync();
    void updateSyncStatus();
    void updateUserInfo(const char* &param);
    void networkConnectionChanged(QNetworkSession::State state);
};

#endif // PCLOUDAPP_H


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
#include "shellextthread.h"
#include "versiontimerthread.h"
#include "welcomescreen.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h" //temp
#include "psynclib.h"
#include <QApplication>
#include <QAction>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QDateTime>
#include <QFile>
#include<QTimer>

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
     QAction *settingsAction;  //Settings page
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
    VersionTimerThread *versnThread;
    bool isFirstLaunch, newVersionFlag;
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg;
    QNetworkSession *session;
    void getQuota();
    void getUserInfo();
#ifdef Q_OS_WIN
     ShellExtThread *shellExtThread;
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
    QString localpathToSync;
    bool rememberMe;
    bool isPremium;
    bool isVerified;
    bool unlinkFlag;
    QString planStr;
    quint64 freeSpacePercentage;
    qreal usedSpace;
   // PSettings *settings;
    QSettings *settings;
    bool upldFlag,downldFlag;
    quint64 bytestoDwnld;
    quint64 bytestoUpld;
    uint32_t lastMessageType; // 0 and 1 for Shares;2 for online status; 3 for newversion
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
    void showLoginPublic();
    void changeSyncIconPublic(const QString &icon); //called from static function status_callbac;  signals are protected and can't be accessed by static vars
    void changeCursorPublic(bool change);
    void updateSyncStatusPublic();
    void updateUserInfoPublic(const char* param);
    void changeOnlineItemsPublic(bool logged);
    void setTextErrPublic(int win , const char *err);
    void addNewSyncPublic();
    void logoutPublic();
    void createSyncFolderActions(QMenu *syncMenu);
    QMenu* getSyncMenu();
    void addNewFolderInMenu(QAction *fldrAction); // refresh menu when add new sync
    QString bytesConvert(quint64 bytes);
    QString timeConvert(quint64 seconds);
    bool isMenuorWinActive();
    void setFirstLaunch(bool b); // case after unlink - to display suggestions
    void check_error();
    bool new_version();
    struct {
        QString url;
        QString notes;
        QString versionstr;
    } newVersion;
signals:
    void showLoginSgnl();
    void changeSyncIcon(const QString &icon);
    void changeCursor(bool change);
    void sendErrText(int win, const char *err);
    void updateSyncStatusSgnl();
    void updateUserInfoSgnl(const char* &param);
    void changeOnlineItemsSgnl(bool logged);
    void addNewSyncSgnl();
    void logoutSignl();
public slots:
    void showTrayMessage(QString title, QString msg);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showAccount();
    //p void showShares();
    void showSync();
    void showSettings();
    void showpCloudAbout();
    void showpcloudHelp();
    //p void openCloudDir();
    //p void shareFolder();
    void logOut();
    void doExit();    
    void trayMsgClicked(); //show shares and new version
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
    void changeOnlineItems(bool logged);
    void check_version();
    void setTimer(int index);
    void networkConnectionChanged(QNetworkSession::State state);
};

#endif // PCLOUDAPP_H


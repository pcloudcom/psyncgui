#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "registerwindow.h"
#include "loginwindow.h"
#include "psettings.h"
//#include "monitoringthread.h"
//#include "onlinethread.h"
//#include "revnotifythread.h"
#include "shellextthread.h"
#include "versiontimerthread.h"
#include "pcloudwindow.h"
#include "notificationsmanager.h"
#include "sharefolderwindow.h"
#include "welcomewin.h"
#include "suggestnsbasewin.h"
#include "infoscreenswin.h"
#include "psynclib.h"
#include <QApplication>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QDateTime>
#include <QFile>
#include <QTimer>


class PCloudApp : public QApplication
{
    Q_OBJECT
private:
    QAction *registerAction;
    QAction *loginAction;
    QAction *exitAction;
    QAction *accountAction, *userinfoAction;
    QAction *driveAction; // opens Drive folder
    QAction *sharesAction; // Shares page
    QAction *cryptoWelcomeAction; //Crypto page, no subscribtion
    QAction *cryptoFldrLockedAction; //Crypto page, folder is Locked
    QAction *cryptoFldrUnlockedAction; // Crypto page, folder is unlocked
    QAction *cryptoOpenFldrAction;
    QAction *cryptoUnlockedMenuAction;
    QAction *shareFolderAction; //opens share folderwin
    QAction *syncAction, *syncSttngsAction; //Sync Tab
    QAction *settingsAction;  //Settings page
    QAction *helpAction; // Help Tab
    QAction *aboutPCloudAction; // About tab
    QAction *pauseSyncAction;
    QAction *resumeSyncAction;
    QAction *addSyncAction;
    QAction *syncDownldAction;
    QAction *syncUpldAction;
    QAction *notfctnsAction;
    QAction *dbgPipeHlprActn; //TEMP
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QMenu * syncMenu, *syncedFldrsMenu, *sharesMenu, *cryptoUnlockedMenu; //submenus in the tray menu
    QSystemTrayIcon *tray;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    ShareFolderWindow *sharefolderwin;
    InfoScreensWin *introwin;
    NotificationsManager *notificationsMngr;
    //MonitoringThread *mthread;
    // VersionTimerThread *versnThread;
    bool newVersionFlag, newNtfFLag;
    QTimer *updateNtfctnTimer;
    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg;
    QNetworkSession *session;
    const char* icons[5][2] = {{OFFLINE_ICON, OFFLINE_ICON_NTF},
                              {SYNCED_ICON,SYNCED_ICON_NTF},
                              {SYNCING_ICON, SYNCING_ICON_NTF},
                              {PAUSED_ICON, PAUSED_ICON_NTF},
                              {SYNC_FULL_ICON, SYNC_FULL_ICON_NTF}};
    int lastTrayIconIndex;
    void getQuota();
    void getUserInfo();
    void getDeskopEnv();
#ifdef Q_OS_WIN
    ShellExtThread *shellExtThread;
    //pRevNotifyThread *notifythread;
#endif
    // OnlineThread *othread;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
public:       
    static PCloudApp *appStatic;
    quint32 lastStatus;
    PCloudWindow *pCloudWin;
    WelcomeWin *welcomeWin;
    SuggestnsBaseWin *syncFldrsWin; // for context menu
    QString username;
    QString downldInfo, uploadInfo;
    bool rememberMe,isFirstLaunch;
    bool isPremium,isVerified;
    bool isCryptoExpired;
    bool unlinkFlag; //clears gui for login after unlink
    bool noFreeSpaceMsgShownFlag; // when come from download or upload thre is a mix of discfull and downlad/upld returned statutes
    bool noEventCallbackFlag; // indicates if event is raised by current user
    bool nointernetFlag;
    QString planStr;
    int freeSpacePercentage;
    QString usedSpaceStr;
    const char* OSStr;
    int fontPointSize;
    QFont smaller2pFont,smaller1pFont, bigger1pFont,bigger2pFont,bigger3pFont;
    // PSettings *settings;
    QSettings *settings;
    QStringList syncSuggstLst; // send from context menu when add new syncs
    QByteArray desktopEnv;
#ifdef Q_OS_WIN
    QSettings *registrySttng;
#endif    
    quint64 bytestoDwnld;
    quint64 bytestoUpld;
    int lastMessageType; // 0 and 1 for Shares;2 for online status; 3 for newversion, init -1
    bool isCursorChanged;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    //p bool isMounted();
    //p void mount();
    //p void unMount();
    void showWindow(QMainWindow *win);
    void logIn(const QString &uname, bool remember);
    //p void setSettings();
    void showError(QString &err);
    void showOnClick();
    bool isLogedIn();
    bool getIsCryptoExpired();
    void setLogWinError(const char *msg); // sets err in logwin; called from static function status_callback
    const QPoint calcWinNextToTrayCoords(const int winWidth, const int winHeigh); //returns TopLeft
    //Public functions ended with 'public' are made to be called from static function status_callback or another threads
    //signals are protected and can't be accessed by static vars
    //in case something in gui thread should change it can't be changed directly from another thread - use signals and slots
    void showLoginPublic();
    void showMsgBoxPublic(QString title, QString msg, int msgIconVal);
    void sendTrayMsgTypePublic(const char* title, const char* msg, int msgtype);
    void changeSyncIconPublic(int index);
    void refreshSyncUIitemsPublic();
    void changeCursorPublic(bool change);
    void updateSyncStatusPublic();
    void updateUserInfoPublic(const char* param);
    void changeOnlineItemsPublic(bool logged);
    void setTextErrPublic(int win, const char *err);
    void addNewSharePublic(QString fldrPath); // from context menu
    void addNewSyncPublic();
    void addNewSyncLstPublic(bool addLocalFldrs); //are local or remote folders are selected
    void lockCryptoFldrPublic();
    void unlockCryptoFldrPublic();
    void updateNotfctnsPublic(int newcnt);
    void setsyncSuggstLst(QStringList lst);
    void logoutPublic();
    void addNewFolderInMenu(QAction *fldrAction); // refresh menu when add new sync
    QString bytesConvert(quint64 bytes);
    QString timeConvert(quint64 seconds);
    bool isMenuorWinActive();
    bool isMainWinPageActive(int index);
    void setFirstLaunch(bool b); // case after unlink - to display suggestions
    void check_error();
    bool new_version();
    void stopTimer();
    struct {
        QString url;
        QString notes;
        QString versionstr;
    } newVersion;
    void removeSetting(QString settingKey);
    void clearUpdtNotifctnSettngs();
    void clearAllSettings();
signals:
    void showLoginSgnl();
    void changeSyncIcon(int index);
    void showMsgBoxSgnl(QString title, QString msg, int msgIconVal);
    void sendTrayMsgType(const char* title, const char* msg, int msgtype);
    void changeCursor(bool change);
    void sendErrText(int win, const char *err);
    void updateSyncStatusSgnl();
    void updateUserInfoSgnl(const char* &param);
    void changeOnlineItemsSgnl(bool logged);
    void addNewShareSgnl(QString fldrPath); //    void addNewShareSgnl(const char* fldrPath);
    void addNewSyncSgnl();
    void addNewSyncLstSgnl(bool addLocalFldrs);
    void lockCryptoFldrSgnl();
    void unlockCryptoFldrSgnl();
    void refreshSyncUIitemsSgnl();
    void updateNotfctnsModelSgnl(int newcnt);
    void logoutSignl();
public slots:
    void showMsgBox(QString title, QString msg, int msgIconVal);
    void showTrayMessage(QString title, QString msg);
    void showTrayMsgType(const char* title, const char* msg, int msgtype);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showAccount();
    void showDrive();
    void showShares();
    void showCrypto();
    void showSync();
    void showSyncSttngs(); // sync page, second tab
    void showSettings();
    void showpCloudAbout();
    void showpcloudHelp();
    void openCloudDir();
    void logOut();
    void unlink();
    void doExit();
    void trayMsgClicked(); //show shares and new version
    //p void setOnlineStatus(bool online);
    void setTrayIcon(int index);
    void refreshTray();
    void setCryptoAction();
    void setCursor(bool change);
    void setErrText(int win, const char *err);
    void pauseSync();
    void resumeSync();
    void openLocalDir(); // for local sync folder
    void lockCryptoFldr();
    void unlockCryptoFldr();
    void openCryptoFldr();
    void addNewShare();
    void addNewShare(QString fldrPath); //void addNewShare(const char* fldrPath); //from context menu
    void addNewSync();
    void addNewSyncLst(bool addLocalFldrs);
    void updateSyncStatus();
    void refreshSyncUIitems();
    void createSyncFolderActions();
    void updateUserInfo(const char* &param);
    void changeOnlineItems(bool logged);
    void check_version();
    void showPopupNewVersion();
    void setTimerInterval(int index);
    void networkConnectionChanged(QNetworkSession::State state);
#ifdef Q_OS_WIN
    // void dbgPipeHlprSLot();
#endif
};

#endif // PCLOUDAPP_H


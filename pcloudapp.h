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
#include "pcloudwindow.h"
#include "welcomewin.h"
#include "suggestnsbasewin.h"
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
   // VersionTimerThread *versnThread;
    bool newVersionFlag;
    QTimer *updateNtfctnTimer;
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
    quint32 lastStatus;
    PCloudWindow *pCloudWin;
    WelcomeWin *welcomeWin;
    SuggestnsBaseWin *syncFldrsWin;
    QString username, authentication;
    QString downldInfo, uplodInfo;
    bool rememberMe,isFirstLaunch;
    bool isPremium;
    bool isVerified;
    bool unlinkFlag; //clears gui for login after unlink
    bool noFreeSpaceMsgShownFlag; // when come from download or upload thre is a mix of discfull and downlad/upld returned statutes
    QString planStr;
    quint64 freeSpacePercentage;
    qreal usedSpace;
    const char* OSStr;
    // PSettings *settings;
    QSettings *settings;
    QStringList syncSuggstLst; // send from context menu when add new syncs
#ifdef Q_OS_WIN
    QSettings *registrySttng;
#endif    
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
    //Public functions ended with 'public' are made to be called from static function status_callback or another threads
    //signals are protected and can't be accessed by static vars
    //in case something in gui thread should change it can't be changed directly from another thread - use signals and slots
    void showLoginPublic();
    void changeSyncIconPublic(const QString &icon);
    void refreshSyncUIitemsPublic();
    void changeCursorPublic(bool change);
    void updateSyncStatusPublic();
    void updateUserInfoPublic(const char* param);
    void changeOnlineItemsPublic(bool logged);
    void setTextErrPublic(int win, const char *err);
    void addNewSyncPublic();
    void addNewSyncLstPublic();
    void setsyncSuggstLst(QStringList lst);
    void logoutPublic();
    void addNewFolderInMenu(QAction *fldrAction); // refresh menu when add new sync
    QString bytesConvert(quint64 bytes);
    QString timeConvert(quint64 seconds);
    bool isMenuorWinActive();
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
    void changeSyncIcon(const QString &icon);
    void showMsgBoxSgnl(QString title, QString msg, int msgIconVal);
    void changeCursor(bool change);
    void sendErrText(int win, const char *err);
    void updateSyncStatusSgnl();
    void updateUserInfoSgnl(const char* &param);
    void changeOnlineItemsSgnl(bool logged);
    void addNewSyncSgnl();
    void addNewSyncLstSgnl();
    void refreshSyncUIitemsSgnl();
    void logoutSignl();
public slots:
    void showMsgBox(QString title, QString msg, int msgIconVal); //for another threads
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
    void unlink();
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
    void addNewSyncLst();
    void updateSyncStatus();
    void refreshSyncUIitems();
    void createSyncFolderActions();
    void updateUserInfo(const char* &param);
    void changeOnlineItems(bool logged);
    void check_version();
    void showPopupNewVersion();
    void setTimerInterval(int index);
    void networkConnectionChanged(QNetworkSession::State state);
};

#endif // PCLOUDAPP_H


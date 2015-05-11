#ifndef CRYPTOPAGE_H
#define CRYPTOPAGE_H
#include "pcloudwindow.h"
#include "pcloudapp.h"
#include "cryptokeydialog.h"

#include <QObject>

class PCloudWindow;
class CryptoKeyDialog;

class CryptoPage : public QObject
{
    Q_OBJECT
    friend class CryptoKeyDialog;
public:
    explicit CryptoPage(PCloudWindow *w, PCloudApp *a,QObject *parent = 0);
    void showEventCrypto();
    void initCryptoPage();
    int getCurrentCryptoPageIndex();
private:
    PCloudApp* app;
    PCloudWindow* win;
    bool tryTrialClickedFlag;
    int pageIndex, passStrenth;
    CryptoKeyDialog *requestCryptoKeyDialog;
    void setCurrentPageIndex();
    void setTrialUI(bool hasSubscriptoin, uint expTime);
    void showSetupCryptoError(int setupRes);
    void showStartCryptoError(int startRes);
    void showMkDirError(int mkdirRes, const char* err);
    void showStopCryptoError(int stopRes);
public slots:
    void lock();
    void unlock();
    void openCryptoFldr();
    void requestCryptoKey();
    void clearSetupUI();
private slots:    
    void autoResize();
    void tryTrial();
    void buyCrypto();
    void getMoreCryptoInfo();
    void setupCrypto();   
    void manageCryptoFldr();
    void setUnlockedFldrUI();
    void setLockedFldrUI();    
    void setProgressBar();
    void checkPasswordsMatch();
};

#endif // CRYPTOPAGE_H

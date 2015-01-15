#ifndef CRYPTOPAGE_H
#define CRYPTOPAGE_H
#include "pcloudwindow.h"
#include "pcloudapp.h"

#include <QObject>

class PCloudWindow;

class CryptoPage : public QObject
{
    Q_OBJECT
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
    void setCurrentPageIndex();
public slots:
    void lock();
    void unlock();
    void openCryptoFldr();
private slots:
    void changePage();
    void tryTrial();
    void buyCrypto();
    void getMoreCryptoInfo();
    void setupCrypto();
    void requestCryptoKey();
    void manageCryptoFldr();
    void setUnlockedFldrUI();
    void setLockedFldrUI();    
    void setProgressBar();
    void checkPasswordsMatch();
};

#endif // CRYPTOPAGE_H

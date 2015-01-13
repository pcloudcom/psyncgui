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
private:
    PCloudApp* app;
    PCloudWindow* win;
    bool tryTrialClickedFlag;
    int pageIndex, passStrenth;
    int getCurrentPageIndex();
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
    void openCryptoFldr();
    void setProgressBar();
    void checkPasswordsMatch();
};

#endif // CRYPTOPAGE_H

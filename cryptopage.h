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
private:
    PCloudApp* app;
    PCloudWindow* win;
    bool showCryptoSttngsFlag, isCryptoActivated, isFirstActivation; //may be to del
    int pageIndex, passStrenth;
    int getCurrentPageIndex();
protected:

signals:

private slots:
    void changePage();
    void tryTrial();
    void buyCrypto();
    void getMoreCryptoInfo();
    void setupCrypto();
    void requestCryptoKey();
    void manageCryptoFldr();
    void setLockedFldrUI();
    void setUnlockFldrUI();
    void openCryptoFldr();
    void setProgressBar();
    void checkPasswordsMatch();
};

#endif // CRYPTOPAGE_H

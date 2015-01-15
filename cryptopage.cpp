#include "cryptopage.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "cryptokeydialog.h"
#include "psynclib.h"
#include <QDateTime>
#include <QUrl>
#include <QDesktopServices>
#include <QPalette>

CryptoPage::CryptoPage(PCloudWindow *w, PCloudApp *a,QObject *parent) :
    QObject(parent)
{
    this->app = a;
    this->win = w;
    passStrenth = -1;

    win->ui->progressBarCryptoPass->setMinimum(0);
    win->ui->progressBarCryptoPass->setMaximum(6);
    win->ui->labelCryptoPassStrenth->setFont(app->smaller1pFont);

    //win->ui->pagedWidgetCrypto->setCurrentIndex(); // TEEEMPPPP
    connect(win->ui->btnNextTest, SIGNAL(clicked()),this, SLOT(changePage())); // TEMPPP
    connect(win->ui->btnNextTest2, SIGNAL(clicked()),this, SLOT(changePage()));
    connect(win->ui->btnNextTest3, SIGNAL(clicked()),this, SLOT(changePage()));
    win->ui->btnNextTest->setVisible(false); //test btns
    win->ui->btnNextTest2->setVisible(false);
    win->ui->btnNextTest3->setVisible(false);

    connect(win->ui->lineEditCryptoPass, SIGNAL(textChanged(QString)), this, SLOT(setProgressBar()));
    connect(win->ui->lineEditCryptoPass2, SIGNAL(textChanged(QString)), this, SLOT(checkPasswordsMatch()));
    connect(win->ui->btnCryptoTryTrial, SIGNAL(clicked()), this, SLOT(tryTrial()));
    connect(win->ui->btnCryptoBuy, SIGNAL(clicked()), this, SLOT(buyCrypto()));
    connect(win->ui->btnCryptoMoreInfo, SIGNAL(clicked()), this, SLOT(getMoreCryptoInfo()));
    connect(win->ui->btnCryptoCreateKey, SIGNAL(clicked()), this, SLOT(setupCrypto()));
    connect(win->ui->btnCryptoManageFldr, SIGNAL(clicked()), this, SLOT(manageCryptoFldr()));
    connect(win->ui->btnCryptoOpenFldr, SIGNAL(clicked()), this, SLOT(openCryptoFldr()));

    // set winxp images
}

void CryptoPage::initCryptoPage() //called when user has just loggedin
{
    tryTrialClickedFlag = false;
    setCurrentPageIndex();
    qDebug()<<"CryptoPage"<<this->pageIndex;
    if (this->pageIndex == 2 && app->settings->value("autostartcrypto").toBool())
        QTimer::singleShot(3000, this, SLOT(requestCryptoKey()));
}

void CryptoPage::showEventCrypto()
{
    setCurrentPageIndex();
    qDebug()<<this->pageIndex;
}

void CryptoPage::setCurrentPageIndex()
{
    qDebug() << " CryptoPage::setCurrentPageIndex" << psync_crypto_isexpired() <<psync_crypto_expires()<< psync_crypto_issetup() << psync_crypto_isstarted();

    int subscbtntTime = psync_crypto_expires();
    if (!tryTrialClickedFlag &&                                                                         //for case when entered pass but hasn't already setup and went to another flag
            (!subscbtntTime || QDateTime::fromTime_t(subscbtntTime) < QDateTime::currentDateTime()))    // trail or subscription expired
    {
        if(!subscbtntTime)
            win->ui->btnCryptoTryTrial->setVisible(true);
        else
            win->ui->btnCryptoTryTrial->setVisible(false);

        app->isCryptoExpired = true;
        this->pageIndex = 0;  //show welcome crypto page
    }
    else if (psync_crypto_issetup() == 0)  //show setup pass page
    {
        if(win->ui->lineEditCryptoPass->text().isEmpty())
        {
            win->ui->progressBarCryptoPass->setValue(0);
            win->ui->labelCryptoPassStrenth->setText("");
            win->ui->lineEditCryptoPass->setFocus();
        }

        app->isCryptoExpired = false;
        this->pageIndex = 1; //show welcome crypto page
    }
    else //show main crypto page
    {
        if(psync_crypto_isstarted())
            setUnlockedFldrUI();
        else
            setLockedFldrUI();

        // ++ check if folder exists
        app->isCryptoExpired = false;
        this->pageIndex = 2;
    }
    win->ui->pagedWidgetCrypto->setCurrentIndex(pageIndex);
}

int CryptoPage::getCurrentCryptoPageIndex()
{
    return this->pageIndex;
}

// slots
void CryptoPage::setUnlockedFldrUI()
{
    win->ui->btnCryptoManageFldr->setText("Lock");
    QPalette paletteRed;
    paletteRed.setColor(QPalette::WindowText, Qt::red);
    win->ui->label_cryptoStatus->setPalette(paletteRed);
    win->ui->label_cryptoStatus->setText("Unlocked");
    win->ui->labelCryptoFldrInfo->setText("Lock the Crypto Folder to keep your files protected.");
    win->ui->btnCryptoOpenFldr->setEnabled(true);
}

void CryptoPage::setLockedFldrUI()
{
    win->ui->btnCryptoManageFldr->setText("Unlock");
    QPalette paletteGreen;
    paletteGreen.setColor(QPalette::WindowText, Qt::green);
    win->ui->label_cryptoStatus->setPalette(paletteGreen);
    win->ui->label_cryptoStatus->setText("Locked");
    win->ui->labelCryptoFldrInfo->setText("Unlock the Crypto Folder to decrypt and manage your files.");
    win->ui->btnCryptoOpenFldr->setEnabled(false);
}

void CryptoPage::changePage() //temp, for tests
{
    QObject *sender = QObject::sender();
    qDebug()<<sender->objectName();
    if (sender->objectName() == "btnNextTest3")
        win->ui->pagedWidgetCrypto->setCurrentIndex(0);

    if (sender->objectName() == "btnNextTest")
        win->ui->pagedWidgetCrypto->setCurrentIndex(1);

    if (sender->objectName() == "btnNextTest2")
        win->ui->pagedWidgetCrypto->setCurrentIndex(2);
}

void CryptoPage::setProgressBar()
{
    passStrenth = psync_password_quality(win->ui->lineEditCryptoPass->text().toUtf8());
    qDebug()<<"setProgressBar "<<passStrenth ;
    QPalette paletteLabel;
    switch(passStrenth)
    {
    case 0:
        win->ui->progressBarCryptoPass->setValue(2);
        win->ui->progressBarCryptoPass->setStyleSheet("QProgressBar:chunk{background-color:#FF4D4D; width: 15px; margin: 0.5px;}");
        paletteLabel.setColor(QPalette::WindowText, QColor("#FF4D4D"));
        win->ui->labelCryptoPassStrenth->setText("Weak");
        break;
    case 1:
        win->ui->progressBarCryptoPass->setValue(4);
        win->ui->progressBarCryptoPass->setStyleSheet("QProgressBar:chunk{background-color:#FF9326; width: 15px; margin: 0.5px;}");
        paletteLabel.setColor(QPalette::WindowText, QColor("#FF9326"));
        win->ui->labelCryptoPassStrenth->setText("Medium");
        break;
    case 2:
        win->ui->progressBarCryptoPass->setValue(6);
        win->ui->progressBarCryptoPass->setStyleSheet("QProgressBar:chunk{background-color:#83C100; width: 15px; margin: 0.5px;}");
        paletteLabel.setColor(QPalette::WindowText, QColor("#83C100"));
        win->ui->labelCryptoPassStrenth->setText("Strong");
        break;
    default:
        break;
    }
    win->ui->labelCryptoPassStrenth->setPalette(paletteLabel);
}

void CryptoPage::checkPasswordsMatch()
{
    if (win->ui->lineEditCryptoPass->text() == win->ui->lineEditCryptoPass2->text())
        win->ui->label_passMatchPic->setPixmap(QPixmap(":/crypto/images/crypto/matchYes.png"));
    else
        win->ui->label_passMatchPic->setPixmap(QPixmap(":/crypto/images/crypto/matchNo.png"));
}

void CryptoPage::tryTrial()
{
    if(!app->isVerified)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(win,"pCloud", "Please verify your email!", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            win->verifyEmail();
        return;
    }

    this->pageIndex = 1;
    win->ui->pagedWidgetCrypto->setCurrentIndex(this->pageIndex);
    tryTrialClickedFlag = true;
}

void CryptoPage::buyCrypto()
{
    QUrl url("https://my.pcloud.com");
    QDesktopServices::openUrl(url);
}

void CryptoPage::getMoreCryptoInfo()
{
    QUrl url("https://my.pcloud.com");
    QDesktopServices::openUrl(url);
}

void CryptoPage::setupCrypto()
{
    if(win->ui->lineEditCryptoPass->text().isEmpty() || win->ui->lineEditCryptoPass2->text().isEmpty())
    {
        QMessageBox::critical(win, "pCloud","Please enter password in both fields!");
        return;
    }

    if(win->ui->lineEditCryptoPass->text() != win->ui->lineEditCryptoPass2->text())
    {
        QMessageBox::critical(win, "Error","The two Crypto Keys should match!");
        return;
    }

    if(win->ui->lineEditCryptoHint->text().isEmpty())
    {
        QMessageBox::critical(win, "Cryto Key Hint","There is no hint provided for the Crypto Key!");
        return;
    }

    if(passStrenth < 1)
    {
        QMessageBox::critical(win, "Crypto Key not strong enough","The Crypto Key is too weak. You need to provide a sufficiant Crypto Key!");
        return;
    }


    int resSetup = psync_crypto_setup(win->ui->lineEditCryptoPass->text().toUtf8(),win->ui->lineEditCryptoHint->text().toUtf8());
    qDebug()<< "CRYPTO: setupCrypto res = " << resSetup;
    if (resSetup == PSYNC_CRYPTO_SETUP_SUCCESS)
    {
        int resCryptoStart = psync_crypto_start(win->ui->lineEditCryptoPass->text().toUtf8());
        qDebug()<<"CRYPTO: startres "<<resCryptoStart;

        const char *err = NULL;
        psync_folderid_t* cryptoFldrId;
        int mkDirRes = psync_crypto_mkdir(0,"Crypto Folder", &err, cryptoFldrId);

        qDebug()<<"CRYPTO: startres "<<resCryptoStart<< "mkdir res = "<<mkDirRes;
        if (!mkDirRes)
        {
            this->showEventCrypto();
            if(tryTrialClickedFlag)
                tryTrialClickedFlag = false;
        }
        else
            qDebug()<<"CRYPTO: Make dir:"<< err;
    }
    else
        qDebug()<< " setupCrypto res gle" << resSetup << psync_get_last_error();
}

void CryptoPage::manageCryptoFldr()
{    
    if(psync_crypto_isstarted())
        lock();
    else
        unlock();
}

void CryptoPage::lock()
{
    int resCryptoManageFLdr = psync_crypto_stop();
    this->setLockedFldrUI();
    qDebug()<<"manageCryptoFldr res " << resCryptoManageFLdr;
}

void CryptoPage::unlock()
{
    emit this->requestCryptoKey();
}


void CryptoPage::requestCryptoKey()
{
    qDebug()<<"requestCryptoKey";
    CryptoKeyDialog *requestCryptoKeyDialog = new CryptoKeyDialog();
    if (requestCryptoKeyDialog->exec() == QDialog::Accepted) // also starts the crypto if pass is ok
    {
        emit this->setUnlockedFldrUI();
    }
}

void CryptoPage::openCryptoFldr()
{
    psync_folderid_t cryptoFldrId = psync_crypto_folderid();
    char *path = psync_fs_get_path_by_folderid(cryptoFldrId);
    qDebug()<<"CRYPTO: openCryptoFldr"<<path<<cryptoFldrId;

    if(path != NULL)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        free(path);
    }
}

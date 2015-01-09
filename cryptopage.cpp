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

    win->ui->pagedWidgetCrypto->setCurrentIndex(1); // TEEEMPPPP
    connect(win->ui->btnNextTest, SIGNAL(clicked()),this, SLOT(changePage())); // TEMPPP
    connect(win->ui->btnNextTest2, SIGNAL(clicked()),this, SLOT(changePage()));
    connect(win->ui->btnCryptoOpenFldr, SIGNAL(clicked()),this, SLOT(changePage()));

    connect(win->ui->lineEditCryptoPass, SIGNAL(textChanged(QString)), this, SLOT(setProgressBar()));
    connect(win->ui->lineEditCryptoPass2, SIGNAL(textChanged(QString)), this, SLOT(checkPasswordsMatch()));
    connect(win->ui->btnCryptoTryTrial, SIGNAL(clicked()), this, SLOT(tryTrial()));
    connect(win->ui->btnCryptoBuy, SIGNAL(clicked()), this, SLOT(buyCripto()));
    connect(win->ui->btnCryptoMoreInfo, SIGNAL(clicked()), this, SLOT(getMoreCryptoInfo()));
    connect(win->ui->btnCryptoCreateKey, SIGNAL(clicked()), this, SLOT(setupCrypto()));
    connect(win->ui->btnCryptoManageFldr, SIGNAL(clicked()), this, SLOT(manageCryptoFldr()));
    connect(win->ui->btnCryptoOpenFldr, SIGNAL(clicked()), this, SLOT(openCryptoFldr()));


    // set winxp images

    this->showEventCrypto();
}

void CryptoPage::showEventCrypto()
{
    this->pageIndex = getCurrentPageIndex();
}

int CryptoPage::getCurrentPageIndex()
{
    qDebug() << " CryptoPage::getCurrentPageIndex" << psync_crypto_isexpired() << psync_crypto_issetup() << psync_crypto_isstarted();

    int subscbtntTime = psync_crypto_isexpired();
    if ( !subscbtntTime || //trial
         QDateTime::fromTime_t(subscbtntTime) < QDateTime::currentDateTime()) // subscription expired
    {
        if(!subscbtntTime)
            win->ui->btnCryptoTryTrial->setVisible(true);
        else
            win->ui->btnCryptoTryTrial->setVisible(false);

        showCryptoSttngsFlag = false;
        return 0; //show welcome crypto page
    }
    else if (psync_crypto_issetup() == 0)  //show setup pass page
    {
        passStrenth = -1;
        win->ui->progressBarCryptoPass->setValue(0);
        //win->ui->progressBarCryptoPass->setStyleSheet("QProgressBar:chunk{background-color: #EEEEEE; width: 15px; margin: 0.5px;}");  // #CD96CDProgressBar:border{16px solid grey;}");
        win->ui->labelCryptoPassStrenth->setText("");
        win->ui->lineEditCryptoPass->setFocus();

        showCryptoSttngsFlag = false;
        return 1;
    }
    else //show main crypto page
    {
        if(psync_crypto_isstarted())
        {
            win->ui->btnCryptoManageFldr->setText("Lock");
            QPalette paletteRed;
            paletteRed.setColor(QPalette::WindowText, Qt::red);
            win->ui->label_cryptoStatus->setPalette(paletteRed);
            win->ui->label_cryptoStatus->setText("Unlocked");
            win->ui->labelCryptoFldrInfo->setText("Lock the Crypto Folder to keep your files protected.");
            win->ui->btnCryptoOpenFldr->setEnabled(true);
        }
        else
        {
            win->ui->btnCryptoManageFldr->setText("Unlock");
            QPalette paletteGreen;
            paletteGreen.setColor(QPalette::WindowText, Qt::green);
            win->ui->label_cryptoStatus->setPalette(paletteGreen);
            win->ui->label_cryptoStatus->setText("Locked");
            win->ui->labelCryptoFldrInfo->setText("Unlock the Crypto Folder to decrypt and manage your files.");
            win->ui->btnCryptoOpenFldr->setEnabled(false);
        }

        // ++ check if folder exists
        showCryptoSttngsFlag = true;
        return 2;
    }
}

bool CryptoPage::getShowCryptoSettingsFlag()
{
    return this->showCryptoSttngsFlag;
}

// slots
void CryptoPage::changePage()
{
    QObject *sender = QObject::sender();
    qDebug()<<sender->objectName();
    if (sender->objectName() == "btnCryptoOpenFldr")
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
    qDebug()<<"checkPasswordsMatch";
    if (win->ui->lineEditCryptoPass->text() == win->ui->lineEditCryptoPass2->text())
        win->ui->label_passMatchPic->setPixmap(QPixmap(":/crypto/images/crypto/matchYes.png"));
    else
        win->ui->label_passMatchPic->setPixmap(QPixmap(":/crypto/images/crypto/matchNo.png"));
}

void CryptoPage::tryTrial()
{
    QUrl url("https://my.pcloud.com");
    QDesktopServices::openUrl(url);
}

void CryptoPage::buyCripto()
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
        QMessageBox::critical(win, "pCloud","Please enter password!");
        return;
    }

    if(passStrenth < 1)
    {
        QMessageBox::critical(win, "pCloud","Password not strong enough!");
        return;
    }

    if(!app->isVerified)
    {
        QMessageBox::critical(win,"pCloud", "Please verify your email!");
        return;
    }
    int resSetup = psync_crypto_setup(win->ui->lineEditCryptoPass->text().toUtf8(),win->ui->lineEditCryptoHint->text().toUtf8());
    qDebug()<< " setupCrypto res " << resSetup;
    if (resSetup == PSYNC_CRYPTO_SETUP_SUCCESS)
        win->ui->pagedWidgetCrypto->setCurrentIndex(2);
}

void CryptoPage::manageCryptoFldr()
{
    int resCryptoManageFLdr;
    if(psync_crypto_isstarted())
    {
        resCryptoManageFLdr = psync_crypto_stop();
        qDebug()<<"manageCryptoFldr res " << resCryptoManageFLdr;
    }
    else
    {
        CryptoKeyDialog* requestCryptoKeyDialog = new CryptoKeyDialog();
        requestCryptoKeyDialog->exec();
    }
}

void CryptoPage::openCryptoFldr()
{

}

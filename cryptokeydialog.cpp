#include "cryptokeydialog.h"
#include "ui_cryptokeydialog.h"
#include "common.h"
#include "psynclib.h"
#include <QMessageBox>
#include <QDebug>

CryptoKeyDialog::CryptoKeyDialog(CryptoPage *cp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CryptoKeyDialog)
{
    ui->setupUi(this);
    this->cryptoPage = cp;

    ui->label_hintVal->setVisible(false);
    setStyleSheet("QToolButton{background-color:transparent; text-decoration: underline;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");

    QPalette Pal(palette());
    QColor frameColor(244,244,243);
    Pal.setColor(QPalette::Background,frameColor);
    ui->frame->setAutoFillBackground(true);
    ui->frame->setPalette(Pal);
    ui->frame->setFrameShadow(QFrame::Plain);
    ui->frame->setFrameShape(QFrame::StyledPanel);
    ui->frame->setMidLineWidth(3);

    connect(ui->btnUnlock, SIGNAL(clicked()), this, SLOT(unlockCrypto()));
    connect(ui->tbtnHint, SIGNAL(clicked()), this, SLOT(setHintLabel()));

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
    this->setWindowTitle(trUtf8("Unlock Crypto Folder"));
    this->setWindowIcon(QIcon(WINDOW_ICON));
}


CryptoKeyDialog::~CryptoKeyDialog()
{
    delete ui;
    delete cryptoPage;
}

void CryptoKeyDialog::unlockCrypto()
{
    if (ui->line_cryptoKey->text().isEmpty())
    {
        QMessageBox::critical(this,"pCloud Drive", "Please enter crypto key");
    }
    else
    {
        int startres = psync_crypto_start(ui->line_cryptoKey->text().toUtf8()); // to check errs
        qDebug()<<"CRYPTO: Unlock start crypto res  = " << startres;
        if(!startres || startres == PSYNC_CRYPTO_START_ALREADY_STARTED)
            this->accept();
        else if(startres == PSYNC_CRYPTO_START_BAD_PASSWORD)
        {
            QMessageBox::critical(this, "Error", "Incorrect Crypto Key!");
            return;
        }
        else
            cryptoPage->showStartCryptoError(startres);
    }
}

void CryptoKeyDialog::setHintLabel()
{
    ui->tbtnHint->setVisible(false);

    char *hint = NULL;
    int getHintRes = psync_crypto_get_hint(&hint);
    if (!getHintRes)
    {
        ui->label_hintVal->setVisible(true);
        if(strlen(hint) < 80)
            ui->label_hintVal->setText(QString("Hint: ").append(hint));
        else
        {
            ui->label_hintVal->setText(QString("Hint: ").append(QString(hint).left(80) + "..."));
            ui->label_hintVal->setToolTip(QString("Hint: ").append(hint));
        }

        free(hint);
    }
    else
       this->showCryptoHintError(getHintRes);
}

void CryptoKeyDialog::showCryptoHintError(int resHint)
{
    qDebug()<<"CRYPTO: hint error = "<<resHint;
    switch(resHint)
    {
    case PSYNC_CRYPTO_HINT_NOT_SUPPORTED:
        QMessageBox::critical(this, "Crypto Error", "Crypto Hint not supported!");
        break;
    case PSYNC_CRYPTO_HINT_NOT_PROVIDED:
        QMessageBox::critical(this, "Crypto Error", "Crypto Hint not provided!");
        break;
    case PSYNC_CRYPTO_HINT_CANT_CONNECT:
        QMessageBox::critical(this, "Crypto Error", "Unable to connect to server.");
        break;
    case PSYNC_CRYPTO_HINT_NOT_LOGGED_IN:
        QMessageBox::critical(this, "Crypto Error", "Your are not logged in!");
        break;
    case PSYNC_CRYPTO_HINT_UNKNOWN_ERROR:
        QMessageBox::critical(this, "Crypto Error", "Unknown error.");
        break;
    default:
        break;
    }
}

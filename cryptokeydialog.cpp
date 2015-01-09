#include "cryptokeydialog.h"
#include "ui_cryptokeydialog.h"
#include "common.h"
#include "psynclib.h"
#include <QMessageBox>
#include <QDebug>

CryptoKeyDialog::CryptoKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CryptoKeyDialog)
{
    ui->setupUi(this);
    ui->label_hintVal->setVisible(false);
    setStyleSheet("QToolButton{background-color:transparent; text-decoration: underline;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");

    connect(ui->btnUnlock, SIGNAL(clicked()), this, SLOT(unlockCrypto()));
    connect(ui->tbtnHint, SIGNAL(clicked()), this, SLOT(setHintLabel()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));

    this->setWindowTitle(trUtf8("Provide Crypto Key"));
    this->setWindowIcon(QIcon(WINDOW_ICON));
}


CryptoKeyDialog::~CryptoKeyDialog()
{
    delete ui;
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
        qDebug()<<"CRYPTO: Unlock res  = " << startres;
        this->accept();
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
        ui->label_hintVal->setText(QString("Hint: ").append(hint));
        free(hint);
    }
    else
        qDebug()<<"CRYPTO: getHintRes = " << getHintRes;
}

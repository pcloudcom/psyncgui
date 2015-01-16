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
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
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
        qDebug()<<"CRYPTO: Unlock start crypto res  = " << startres;
        if(!startres)
            this->accept();
        else if(startres == PSYNC_CRYPTO_START_BAD_PASSWORD)
        {
            QMessageBox::critical(this, "Error", "Incorrect Crypto Key!");
            return;
        }
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
        if(strlen(hint) < 40)
            ui->label_hintVal->setText(QString("Hint: ").append(hint));
        else
        {
            ui->label_hintVal->setText(QString("Hint: ").append(QString(hint).left(40) + "..."));
            ui->label_hintVal->setToolTip(QString("Hint: ").append(hint));
        }

        free(hint);
    }
    else
        qDebug()<<"CRYPTO: getHintRes = " << getHintRes;
}

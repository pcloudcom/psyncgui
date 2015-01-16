#ifndef CRYPTOKEYDIALOG_H
#define CRYPTOKEYDIALOG_H
#include "cryptopage.h"

#include <QDialog>

namespace Ui {
class CryptoKeyDialog;
}

class CryptoPage;

class CryptoKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CryptoKeyDialog(CryptoPage *cp,QWidget *parent = 0);
    ~CryptoKeyDialog();
private:
    Ui::CryptoKeyDialog *ui;
    CryptoPage* cryptoPage;
    void showCryptoHintError(int resHint);
private slots:
    void setHintLabel();
    void unlockCrypto();
};

#endif // CRYPTOKEYDIALOG_H

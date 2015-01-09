#ifndef CRYPTOKEYDIALOG_H
#define CRYPTOKEYDIALOG_H

#include <QDialog>

namespace Ui {
class CryptoKeyDialog;
}

class CryptoKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CryptoKeyDialog(QWidget *parent = 0);
    ~CryptoKeyDialog();

private:
    Ui::CryptoKeyDialog *ui;
private slots:
    void setHintLabel();
    void unlockCrypto();
};

#endif // CRYPTOKEYDIALOG_H

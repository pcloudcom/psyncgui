#ifndef CHANGEPASSDIALOG_H
#define CHANGEPASSDIALOG_H

#include <QDialog>

namespace Ui {
class ChangePassDialog;
}

class ChangePassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePassDialog(QWidget *parent = 0);
    ~ChangePassDialog();

private:
    Ui::ChangePassDialog *ui;
};

#endif // CHANGEPASSDIALOG_H

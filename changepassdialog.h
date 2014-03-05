#ifndef CHANGEPASSDIALOG_H
#define CHANGEPASSDIALOG_H
#include "pcloudwindow.h"
#include <QDialog>

namespace Ui {
class ChangePassDialog;
}


class ChangePassDialog : public QDialog
{
    Q_OBJECT

public:
    friend class PCloudWindow;
    explicit ChangePassDialog(QWidget *parent = 0);
    ~ChangePassDialog();

private:
    Ui::ChangePassDialog *ui;
public slots:
    void checkPasses();
};

#endif // CHANGEPASSDIALOG_H

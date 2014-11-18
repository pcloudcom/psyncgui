#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
class LoginWindow;
}

class PCloudApp;
//class Sync;
class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(PCloudApp *a, QWidget *parent = 0);
    ~LoginWindow();
    int test;
private:
    PCloudApp *app;
    Ui::LoginWindow *ui;
    QString username;
    void closeEvent(QCloseEvent *event);
protected:
    void showEvent(QShowEvent *);
public slots:
    void logIn();
    void forgotPassword();
    void focusPass();
    void unlinkSync();
    void showError(const char *err);
};

#endif // LOGINWINDOW_H


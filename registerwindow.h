#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QShowEvent>

namespace Ui {
class RegisterWindow;
}

class PCloudApp;

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(PCloudApp *a, QWidget *parent = 0);
    ~RegisterWindow();
private:
    PCloudApp *app;
    Ui::RegisterWindow *ui;
    void setError(const char *err);
    void closeEvent(QCloseEvent *event);
protected:
    void showEvent(QShowEvent *event);
public slots:
    void focusPass();
    void focusConfirm();
    void focusTOS();
    void doRegister();
};

#endif // REGISTERWINDOW_H

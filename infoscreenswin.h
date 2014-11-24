#ifndef INFOSCREENSWIN_H
#define INFOSCREENSWIN_H

#include <QMainWindow>
#include <QShowEvent>

namespace Ui {
class InfoScreensWin;
}

class PCloudApp;

class InfoScreensWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit InfoScreensWin(PCloudApp *a,QWidget *parent = 0);
    ~InfoScreensWin();
public slots:
    void createDfltSync();
    void changePageContent(int index);
    void openPreviousPage();
    void openNextPage();
    void finish();    
private:
    Ui::InfoScreensWin *ui;
    PCloudApp *app;
protected:
    void showEvent(QShowEvent *event);
};

#endif // INFOSCREENSWIN_H

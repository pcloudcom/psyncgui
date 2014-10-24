#ifndef INFOSCREENSWIN_H
#define INFOSCREENSWIN_H

#include <QMainWindow>

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
    void changePageContent(int index);
    void openPreviousPage();
    void openNextPage();
    void finish();    
private:
    Ui::InfoScreensWin *ui;
    PCloudApp *app;
};

#endif // INFOSCREENSWIN_H

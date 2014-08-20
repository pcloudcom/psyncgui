#ifndef SHAREFOLDERWINDOW_H
#define SHAREFOLDERWINDOW_H

#include "common.h"
#include "psynclib.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QShowEvent>s

namespace Ui {
class ShareFolderWindow;
}

class SharesPage;
class PCloudWindow;

class ShareFolderWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    //friend class SharesPage; //
    explicit ShareFolderWindow(PCloudWindow *w,SharesPage *sp, QWidget *parent = 0);
    ~ShareFolderWindow();
private:
    Ui::ShareFolderWindow *ui;
    SharesPage* sharePage;
    PCloudWindow* pclwin;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
public slots:
    void dirSelected(QTreeWidgetItem *dir);
    void shareFolder();
    void showError(const char* err);
};

#endif // SHAREFOLDERWINDOW_H

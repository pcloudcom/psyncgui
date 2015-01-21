#ifndef SHAREFOLDERWINDOW_H
#define SHAREFOLDERWINDOW_H

#include "common.h"
#include "psynclib.h"
#include "remotetreesdialog.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QShowEvent>

namespace Ui {
class ShareFolderWindow;
}

class PCloudWindow;
class PCloudApp;

class ShareFolderWindow : public QMainWindow
{
    Q_OBJECT
    
public:    
    explicit ShareFolderWindow(PCloudApp *a,PCloudWindow *w, QString path, QWidget *parent = 0);
    ~ShareFolderWindow();
    void setContextMenuFlag(bool flag);   
    void setFldrbyMenu(QString path); // for contex menu only
private:
    Ui::ShareFolderWindow *ui;
    PCloudApp *app;
    bool contxMenuFlag;
    QString fldrPath;
    quint64 fldrid;
    PCloudWindow* pclwin;
    RemoteTreesDialog *remoteFldrsDialog;
    void displayShareName();
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
public slots:
    void setFlrd();    
    void shareFolder();
    void showError(const char* err);
};

#endif // SHAREFOLDERWINDOW_H

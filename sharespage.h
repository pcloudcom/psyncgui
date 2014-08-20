#ifndef SHARESPAGE_H
#define SHARESPAGE_H

#include "changepermissionsdialog.h"
#include "acceptsharedialog.h"
#include "sharefolderwindow.h"
#include <QObject>
#include <QTreeWidgetItem>
#include <QMessageBox>

class PCloudApp;
class PCloudWindow; // allows access to ui

class SharesPage : public QObject
{
    Q_OBJECT
public:
    friend class PCloudWindow;    
    explicit SharesPage(PCloudWindow *w, PCloudApp *a, QObject *parent = 0);
    ~SharesPage();
    void selectErr();
    void loadAll();
private:
    PCloudWindow *win;
    PCloudApp *app;
    ShareFolderWindow *sharefolderwin;
    QString getPermissions(quint8 perm);
    void fillSharesTable(bool incoming);
    void fillRequestsTable(bool incoming);
    void addSharesRow(QTreeWidget *table,QStringList data, quint64 id, quint8 perms, int index);
    void setTableProps(QTreeWidget* table);
    void getError(int res, char* err);

public slots:
    void refreshTab(int i);
    void stopShare();
    void modifyShare();
    void cancelRqst(); //cancel requests
    void acceptRqst(); //incoming shares
    void shareFolder();   
};


#endif // SHARESPAGE_H

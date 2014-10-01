#ifndef SHARESPAGE_H
#define SHARESPAGE_H

#include "changepermissionsdialog.h"
#include "acceptsharedialog.h"
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
    void setRequestsVisibility(int incoming, bool visible);
private:
    PCloudWindow *win;
    PCloudApp *app;    
    QString getPermissions(quint8 perm);
    void fillSharesTable(bool incoming);
    void fillRequestsTable(bool incoming);
    void addSharesRow(QTreeWidget *table,QStringList data, quint64 id, quint64 fldrid,quint8 perms, int index);
    void setTableProps(QTreeWidget* table);
    void getError(int res, char* err);
public slots:
    void refreshTab(int i);
    void stopShare();
    void modifyShare();
    void cancelRqst(); //cancel requests
    void acceptRqst(); //incoming shares    
    void openSharedFldr(QTreeWidgetItem* item,int index);
};


#endif // SHARESPAGE_H

#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H  /// to CHANGE !!!!!!

//#include "addsyncdialog.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QStringList>
#include <QItemDelegate>
#include <QTableWidget>
#include <QTreeWidgetItem>


class PCloudApp;

namespace Ui {
class SuggestnsBaseWin;
}

class SuggestnsBaseWin : public QMainWindow
{
    Q_OBJECT

public:
    friend class addSyncDialog;
    friend class SyncItemsDelegate;
    explicit SuggestnsBaseWin(PCloudApp *a, QStringList *fldrs, QWidget *parent = 0);
    ~SuggestnsBaseWin();
    QStringList remoteFldrsNamesLst, newRemoteFldrsLst;
    QString getCurrLocalPath();
    QString getCurrRemotePath();
    int getCurrType();
    QTreeWidgetItem* getCurrItem();
    void addNewItem(QString &localpath, QString &remotepath, int type);
    void addLocalFldrs(QStringList *itemsLst);
    bool getChangeItem();
    void setChangeItem(bool);
    QString checkRemoteName(QString &entryName);
    void addNewRemoteFldr(QString &name);
private:
    QStringList *localFldrsLst;
    //indicates if user adds a new sync or changes a suggested one
protected:    
    //Ui::
    Ui::SuggestnsBaseWin *ui;
    void closeEvent(QCloseEvent *event);
    PCloudApp *app;
    QString currentLocal, currentRemote;
    int currentType;
    bool isChangingItem;    
public slots:
    void addSync();
    void changeCurrItem(QModelIndex index);
    void modifyType();
    void finish();
};

#endif // WELCOMESCREEN_H

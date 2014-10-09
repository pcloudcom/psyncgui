#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H  /// to CHANGE !!!!!!

#include <QMainWindow>
#include <QCloseEvent>
#include <QStringList>
#include <QItemDelegate>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QDir>
#include <QStandardPaths>

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
    explicit SuggestnsBaseWin(PCloudApp *a, bool addlocal, QStringList *fldrs, QWidget *parent = 0);
    ~SuggestnsBaseWin();
    QStringList remoteFldrsNamesLst, newRemoteFldrsLst;
    QString getCurrLocalPath();
    QString getCurrRemotePath();
    int getCurrType();
    QTreeWidgetItem* getCurrItem();
    void addNewItem(QString &localpath, QString &remotepath, int type);    
    void addLocalFldrs(QStringList *itemsLst);
    void addRemoteFldrs(QStringList *itemsLst);
    bool getChangeItem();
    void setChangeItem(bool);
    QString checkRemoteName(QString &entryName);    
    void addNewRemoteFldr(QString &name);
private:
    bool addLocalFldrsFlag;
    QStringList *addFldrsLst;  //indicates if user adds a new sync or changes a suggested one
    QString getLocalName(QString &entryName);//, QDir &usrDir);
protected:        
    Ui::SuggestnsBaseWin *ui;
    void closeEvent(QCloseEvent *event);
    PCloudApp *app;
    QDir *dfltDir;
    QString currentLocal, currentRemote;
    int currentType;
    bool isChangingItem;    
public slots:
    void addSync();
    virtual void changeCurrItem(QModelIndex index);
    void modifyType();
    void finish();
};

#endif // WELCOMESCREEN_H

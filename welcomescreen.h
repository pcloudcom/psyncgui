#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include "addsyncdialog.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QItemDelegate>
#include <QTableWidget>
#include <QTreeWidgetItem>

namespace Ui {
class WelcomeScreen;
}
class PCloudApp;

class WelcomeScreen : public QMainWindow
{
    Q_OBJECT

public:
    friend class addSyncDialog;
    friend class SyncItemsDelegate;
    explicit WelcomeScreen(PCloudApp *a,QWidget *parent = 0);
    ~WelcomeScreen();
    QStringList remoteFldrsNamesLst, newRemoteFldrsLst;
    QString getCurrLocalPath();
    QString getCurrRemotePath();
    int getCurrType();
    QTreeWidgetItem* getCurrItem();
    void load();
    void addNewItem(QString localpath, QString remotepath, int type);
    bool getChangeItem();
    void setChangeItem(bool);
    QString checkRemoteName(QString entryName);
  //  QString checkLocalPathExists(QString path,QString dirName);
private:
    Ui::WelcomeScreen *ui;
    PCloudApp *app;
    QString currentLocal, currentRemote;
    int currentType;
    bool isChangingItem; //indicates if user adds a new sync or changes a suggested one
  //  QList<int> syncsToAdd;
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void addSync();
    void changeCurrItem(QModelIndex index);
    void testPrint(QModelIndex index);
    void testPrintTree(QTreeWidgetItem*itm,int col);
 //   void chClicked(QModelIndex i1,QModelIndex i2);
   // void comboClicked(int index);
    void modifyType();
    void finish();
};

#endif // WELCOMESCREEN_H

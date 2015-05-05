#ifndef NOTIFICATIONSMANAGER_H
#define NOTIFICATIONSMANAGER_H

#include "notifydelegate.h"
#include "psynclib.h"
#include <QObject>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>
#include <QLabel>
#include <QPaintEvent>
//#include <QHideEvent>
//#include <QMouseEvent>
//#include <QFocusEvent>

class PCloudApp;
class NotificationsManager;

class NotificationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationsWidget(NotificationsManager *mngr, int height, QWidget *parent = 0);
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
    //virtual void leaveEvent(QEvent *event);
    //virtual void hideEvent(QHideEvent *event);
    //virtual void focusOutEvent(QFocusEvent *event);
    //virtual void mousePressEvent(QMouseEvent *event);
private:
    NotificationsManager *mngrParent;
    //NotifyDelegate *delegate;
};


class CntrWidget : public QWidget //the orange icon counter
{
    Q_OBJECT
public:
    explicit CntrWidget(QFont fontVal, QWidget *parent = 0);
    void setNumNew(int newValue);
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    int numNew, radius;
    QFont cntrFont;
};


struct actionsManager
{
    quint8 actionId;
    psync_notification_action_t actionData;

    actionsManager()
    {
        actionId = PNOTIFICATION_ACTION_NONE;
    }
    actionsManager(quint8 id, psync_notification_action_t data)
    {
        actionId = id;
        actionData = data;
    }
};

class NotificationsManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsManager(PCloudApp *a, QObject *parent = 0);
    bool updateFlag;
    void init();
    void clear();
    quint32 getLastNtfctId();
    void resetNums();
private:
    quint8 numRead, dtFontSize;
    PCloudApp* app;
    actionsManager* actnsMngrArr;
    NotifyDelegate *notifyDelegate;
    QString  textHtmlBeginStr,textHtmlEndStr, dtHtmlBeginStr, dtHtmlEndStr, fldrIconPath;
    QLabel *noNtfctnsLabel;
    CntrWidget *cntrWid;
    QTableView* table;
    bool hasTableScrollBar;
    NotificationsWidget *notifywin;
    QVBoxLayout *layout;
    QHBoxLayout *hlayout;
    QStandardItemModel *notificationsModel;
    quint32 lastNtfctId;
    void setTableProps();
    void loadModel(psync_notification_list_t* notifications);
    void clearModel();
signals:
private slots:

public slots:
    void setWinFocus();
    void showNotificationsWin();
    void updateNotfctnsModel(int newcnt);
    void actionExecSlot(const QModelIndex &index);
};

#endif // NOTIFICATIONSMANAGER_H

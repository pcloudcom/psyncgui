#ifndef NOTIFICATIONSMANAGER_H
#define NOTIFICATIONSMANAGER_H

#include "notifydelegate.h"
#include <QObject>
#include <QStandardItemModel>
#include <QTableView>
//#include <QTableWidget>
//#include <QImage>
//#include <QTextDocument>
#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>
#include <QLabel>
//#include <QPushButton>


class NotificationsManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsManager(QObject *parent = 0);
    void initModel();
    void updateModel();

private:
    int numRead;
    QTableView* table;
    QWidget *notifywin;
      QVBoxLayout *layout;
       QHBoxLayout * hlayout;
    QStandardItemModel *notificationsModel;
    NotifyDelegate *notifyDelegate;
signals:

public slots:
     void showNotificationsWin();
};

#endif // NOTIFICATIONSMANAGER_H

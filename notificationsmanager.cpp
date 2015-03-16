#include "notificationsmanager.h"

#include <QDebug>

NotificationsManager::NotificationsManager(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"NotificationsManager create";
    numRead = 0;
    table = new QTableView();
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setShowGrid(false);
    table->viewport()->setAttribute(Qt::WA_Hover);
    table->setMouseTracking(true);
    table->setMinimumHeight(400);
    QHeaderView *headerH = table->horizontalHeader(), *headerV = table->verticalHeader();
    headerH->hide();
    headerV->hide();

    notificationsModel = new QStandardItemModel(10, 2);
    table->setModel(notificationsModel);
    notifyDelegate = new NotifyDelegate(table); //
    table->setItemDelegate(notifyDelegate);
    this->initModel(); //TEMPMODEL
    layout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    QLabel *label = new QLabel();
    label->setTextFormat(Qt::RichText);
    label->setText("<html><head/><body><p><span style=\" font-weight:600;\">Richtext <br> newline</span></p></body></html>");
    hlayout->addWidget(label);
    layout->addLayout(hlayout);
    layout->addWidget(table);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    notifywin = new QWidget;
    notifywin->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    notifywin->setLayout(layout);
    notifywin->setMinimumWidth(400);
    qDebug()<<"NotificationsManager end";
    notifywin->show();

}

void NotificationsManager::initModel()
{
    //notificationsModel->setRowCount(10);
    for (int row = 0; row < notificationsModel->rowCount()-1; ++row)
    {
        {
            QModelIndex index = notificationsModel->index(row, 1, QModelIndex());
            //QString value = "<html><head/><body> <ul> <li>Secure place for private/business use</li> <li>Zero-Knowledge Privacy</li> </ul></body></html>"; //"test<b>fdsfdsfs</b>";
            QString value;
            if (row%2)
            value = "<html><head/><body><p><span style=\" font-weight:600;\">User dlfdlsfjlsfjlsfjlsfjlsfjdlssadjkas@afd.bg<br>edited file blqbql<br>in folder lqlq2</span></p></body></html>"
                  "      <p><span style=\" font-size:9pt; color:#2bc1d1;\">   dateexm</span></p></body></html>";
            else
                value = "<html><head/><body><p><span style=\" font-weight:600;\">User dlsadjcdfsfsfdfgskas@afd.bg<br>edited file blqbql<br>in folder lqlq2</span></p></body></html>"
            "<p><span style=\" font-size:9pt; color:#2bc1d1;\">   dateexm<br>нов ред<br>another one</span></p></body></html>";

            notificationsModel->setData(index, QVariant(value),Qt::EditRole);
            notificationsModel->setData(index,QVariant("datatest"), Qt::UserRole+1);
            QModelIndex indexIcon = notificationsModel->index(row, 0, QModelIndex());
            QString iconpath = "/home/damyanka/git/psyncguiSeptm/psyncgui/images/menu 48x48/info.png";
            notificationsModel->setData(indexIcon, QVariant(iconpath));
        }
    }

    for (int i = 0; i < notificationsModel->rowCount(); ++i)
    {
        table->openPersistentEditor(notificationsModel->index(i, 1)) ;
        table->openPersistentEditor(notificationsModel->index(i, 0)) ;
    }
}

void NotificationsManager::updateModel()
{

}

void NotificationsManager::showNotificationsWin()
{
    notifywin->show();
    //if !num - set dflt text, hide table
    //notifywin->raise();
    //notifywin->activateWindow();
    //notifywin->showNormal();
    //notifywin->setWindowState(Qt::WindowActive);
    //a.setActiveWindow(notifywin);
}

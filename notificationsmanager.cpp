#include "notificationsmanager.h"
#include "pcloudapp.h"

#include <QDebug>
#include <QCursor>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>

NotificationsWidget::NotificationsWidget(NotificationsManager *mngr, QWidget *parent) : QWidget(parent)
{
    //setFocusPolicy(Qt::ClickFocus);
    //setFocusPolicy((Qt::FocusPolicy)(Qt::TabFocus|Qt::ClickFocus));
    this->setWindowFlags(Qt::Dialog);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    this->setMinimumWidth(420);
    this->mngrParent = mngr;
    this->installEventFilter(this);
}

/*
void NotificationsWidget::leaveEvent()
{
    qDebug()<<"NotificationsWidget::leaveEvent";
}

void NotificationsWidget::hideEvent(QHideEvent *event)
{
    qDebug()<<"hideEvent";
     bool res = this->close();
     qDebug()<<res;
     event->accept();
}
*/

bool NotificationsWidget::eventFilter(QObject *watched, QEvent *event)
{
    // paint repaint show(51) actChange(99)
    if(event->type() == QEvent::WindowDeactivate) //25
    {
        qDebug()<<"eventFilter win deactivate";
        this->close();

        if(mngrParent->getLastNtfctId() != -1)
        {
            psync_mark_notificaitons_read(mngrParent->getLastNtfctId()); //++ check errors
            mngrParent->resetNums();
        }
    }

    QWidget::eventFilter(watched, event);
}

/*
void NotificationsWidget::mouseEvent(QMouseEvent *event)
{
    qDebug()<<"mousePressEvent";
    QPoint pos = event->globalPos();
   if (this)
   pos = this->mapFromGlobal(pos);
   if (!this->rect().contains(pos))
       this->close();
  // hideTip();
   QWidget::mouseMoveEvent(event);
}

void NotificationsWidget::focusOutEvent(QFocusEvent *event) // doesn't work when alt+tab and the mouse is over ttable item
{
   // qDebug()<<"focusOutEvent"<< this->isVisible();
    QPoint pos = QCursor::pos(); //25 QEvent::WindowDeactivate
    if (this)
        pos = this->mapFromGlobal(pos);

    if (!this->rect().contains(pos) || !this->isVisible())
    {
        this->close();

        if(mngrParent->getLastNtfctId() != -1)
        {
            psync_mark_notificaitons_read(mngrParent->getLastNtfctId()); //++ check errors
            mngrParent->resetNums();
        }
    }
    event->accept();
}
*/


CntrWidget::CntrWidget(QWidget *parent) :QWidget(parent)
{
    this->numNew = 0;
    this->setMaximumSize(QSize(24,24));
}

void CntrWidget::paintEvent(QPaintEvent *event)
{
    //   qDebug()<<"CntrWidget::paintEvent";
    if(!this->numNew)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor color("#FF7040");
    painter.setPen(color);
    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    painter.setBrush(brush);
    painter.drawEllipse(event->rect().center(),10,10);
    //painter.drawArc(event->rect(),0,360*16);
    painter.setPen(Qt::white);
    //painter.setFont();
    painter.drawText(event->rect(), Qt::AlignCenter, QString::number(this->numNew));
}

void CntrWidget::setNumNew(int newValue)
{
    this->numNew = newValue;
}

NotificationsManager::NotificationsManager(PCloudApp *a, QObject *parent) :
    QObject(parent)
{
    qDebug()<<"NotificationsManager create";
    numRead = 0;
    app = a;
    actnsMngrArr = NULL;
    updateFlag = false;
    lastNtfctId = -1;

    if(app->font().pointSize() > 10)
        dtFontSize = app->fontPointSize -3;
    else
        dtFontSize = app->fontPointSize -1;

    //Ü7 - for tests
    textHtmlBeginStr = QString("<html></title><body><p style = \"margin:0px;\">");
    textHtmlEndStr = QString("</p>");
    dtHtmlBeginStr = QString("<p style = \"margin-top:8px;margin-bottom:0px;margin-left:0px;margin-right:0px;font-size:").append(QString::number(dtFontSize)).append("pt; color:#797979;\">");
    dtHtmlEndStr = QString("</p></body></html>");
    table = new QTableView();
    this->setTableProps();

    notificationsModel = new QStandardItemModel(0, 2);
    table->setModel(notificationsModel);
    notifyDelegate = new NotifyDelegate(table);
    table->setItemDelegate(notifyDelegate);

    layout = new QVBoxLayout();
    hlayout = new QHBoxLayout();
    QLabel *label = new QLabel(), *icon = new QLabel();
    cntrWid = new CntrWidget();
    label->setText(" pCloud Notifications");
#ifdef Q_OS_LINUX
    label->setFont(app->bigger3pFont);
#else
    label->setFont(app->bigger1pFont);
#endif

    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    icon->setPixmap(QPixmap(":/48x34/images/48x34/notify.png"));
    icon->setMaximumWidth(68);
    icon->setMaximumHeight(80);
    icon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    hlayout->addWidget(icon);
    hlayout->addWidget(label);
    hlayout->addWidget(cntrWid);
    layout->addLayout(hlayout);
    noNtfctnsLabel = new QLabel("No notifications available.");
    noNtfctnsLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    noNtfctnsLabel->setMargin(50);
    noNtfctnsLabel->setVisible(false);
    layout->addWidget(noNtfctnsLabel);
    layout->addWidget(table);
    notifywin = new NotificationsWidget(this);
    notifywin->setLayout(layout);
    //notifywin->show(); //for dbg
    //notifywin->setFocus();

    // connect(table, SIGNAL(viewportEntered()), this, SLOT(setWinFocus()));
    //connect(table, SIGNAL(clicked(QModelIndex)), this, SLOT(setWinFocus()));
    connect(table, SIGNAL(clicked(QModelIndex)), this, SLOT(actionExecSlot(QModelIndex)));
    //connect(table, SIGNAL(entered(QModelIndex)), this, SLOT(setWinFocus()));

    //connect(table->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(setWinFocus()));
    //connect(table->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(setWinFocus()));

}

void NotificationsManager::setTableProps()
{
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setContentsMargins(0,0,0,0);
    table->setStyleSheet("QTableView{background-color:#F3F3F3;}");
    table->setShowGrid(false);
    table->viewport()->setAttribute(Qt::WA_Hover);
    table->setMouseTracking(true);
    table->setMinimumHeight(400);
    QHeaderView *headerH = table->horizontalHeader(), *headerV = table->verticalHeader();
    headerH->hide();
    headerV->hide();
    table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

void NotificationsManager::loadModel(psync_notification_list_t* notifications)
{
    qDebug()<<"loadmodel"<<notifications->newnotificationcnt <<notifications->notificationcnt<<notifications->notifications;

    int ntfCnt = notifications->notificationcnt;
    notificationsModel->setRowCount(ntfCnt);
    actnsMngrArr = new actionsManager[ntfCnt];

    cntrWid->setNumNew(notifications->newnotificationcnt);
    cntrWid->update();
    this->notifyDelegate->setNumNew(notifications->newnotificationcnt);
    lastNtfctId = notifications->notifications[0].notificationid;

    for (int i = 0; i < ntfCnt; i++)
    {
        //        qDebug()<< notifications->notifications[i].actiondata.folderid <<notifications->notifications[i].actionid <<
        //                 "iconid="<< notifications->notifications[i].iconid<<"actionid"<<notifications->notifications[i].isnew<<
        //               notifications->notifications[i].mtime << notifications->notifications[i].notificationid
        //          <<notifications->notifications[i].text <<notifications->notifications[i].thumb;

        QModelIndex indexHtml = notificationsModel->index(i, 1, QModelIndex());
        QString htmldata(textHtmlBeginStr + notifications->notifications[i].text + textHtmlEndStr);
        htmldata.append(dtHtmlBeginStr).append((QDateTime::fromTime_t(notifications->notifications[i].mtime).toString())).append(dtHtmlEndStr);
        notificationsModel->setData(indexHtml, QVariant(htmldata), Qt::EditRole); //displayrole

        actnsMngrArr[i].actionId = notifications->notifications[i].actionid;
        if(actnsMngrArr[i].actionId)
            actnsMngrArr[i].actionData = notifications->notifications[i].actiondata;
        //        qDebug()<<"actnsMngrArr"<<actnsMngrArr[i].actionId<<actnsMngrArr[i].actionData.folderid;

        QModelIndex indexIcon = notificationsModel->index(i, 0, QModelIndex());
        QString iconpath;
        if (notifications->notifications[i].thumb != NULL)
            iconpath = notifications->notifications[i].thumb;
        else
            //iconpath = dfltname+ notifications->notifications[i].iconid
            iconpath = ":/images/images/testNtf.png";
        notificationsModel->setData(indexIcon, QVariant(iconpath));

        table->openPersistentEditor(notificationsModel->index(i, 1));
        table->openPersistentEditor(notificationsModel->index(i, 0));

        table->resizeRowsToContents();
    }
}

void NotificationsManager::clearModel()
{
    notificationsModel->removeRows(0, notificationsModel->rowCount());

    if (actnsMngrArr != NULL)
    {
        free(actnsMngrArr);
        actnsMngrArr = NULL;
    }
}

void NotificationsManager::init()
{
    psync_notification_list_t* notifications = psync_get_notifications();
    if(notifications != NULL && notifications->notificationcnt)
    {
        this->loadModel(notifications);
        free(notifications);
        notifications = NULL;
    }
    else
    {
        qDebug()<<"initmodel no notif";
        this->table->setVisible(false);
        this->noNtfctnsLabel->setVisible(true);
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();

    /*
     * OLD INIT TESTING MODEL
    notificationsModel->setRowCount(10);
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
                //  value = "<html><head/><body><p><span style=\" font-weight:600;\">User dlsadjcdfsfsfdfgskas@afd.bg<br>edited file blqbql<br>in folder lqlq2</span></p></body></html>"
                //        "<p><span style=\" font-size:9pt; color:#2bc1d1;\">   dateexm<br>нов ред<br>another one</span></p></body></html>";
                value = "We must be <b>bold</b>, very <b>bold</b><br><p><span style=\" font-size:9pt; color:#2bc1d1;\">   dateexm</span></p>";

            notificationsModel->setData(index, QVariant(value),Qt::EditRole);
            notificationsModel->setData(index,QVariant("datatest"), Qt::UserRole+1);
            QModelIndex indexIcon = notificationsModel->index(row, 0, QModelIndex());
            //QString iconpath = "/home/damyanka/git/psyncguiSeptm/psyncgui/images/menu 48x48/info.png";
            QString iconpath = "/home/damyanka/git/psyncguiSeptm/psyncgui/images/testNtf.png";
            notificationsModel->setData(indexIcon, QVariant(iconpath));
        }
    }


    for (int i = 0; i < notificationsModel->rowCount(); ++i)
    {
        table->openPersistentEditor(notificationsModel->index(i, 1)) ;
        table->openPersistentEditor(notificationsModel->index(i, 0)) ;
    }
    */
}

void NotificationsManager::clear()
{
    this->clearModel();
    this->resetNums();
}

void NotificationsManager::updateNotfctnsModel(int newcnt)
{
    // if(!notifywin->isVisible())
    //  if(this->lastNtfctId != -1 && newcnt) // callbacked is called after marking red
    if(newcnt) // callbacked is called after marking red
    {
        psync_notification_list_t* notifications = psync_get_notifications();

        //numread = new
        qDebug()<<"updateNotfctnsModel"<< notifications->newnotificationcnt << notifications->notificationcnt;
        if (notifications != NULL && notifications->newnotificationcnt) //first notifications
        {
            if(!table->isVisible())
            {
                noNtfctnsLabel->setVisible(false);
                table->setVisible(true);
            }

            this->clear();
            this->loadModel(notifications);

            /*     for (int i = 0; i < cnt; i++)
        {
            qDebug()<< notifications->notifications[i].actiondata.folderid <<notifications->notifications[i].actionid <<
                       notifications->notifications[i].iconid<<notifications->notifications[i].isnew<<
                       notifications->notifications[i].mtime << notifications->notifications[i].notificationid
                    <<notifications->notifications[i].text <<notifications->notifications[i].thumb;

            QModelIndex indexHtml = notificationsModel->index(i, 1, QModelIndex());
            //QString htmldata("<b>");
            QString htmldata(notifications->notifications[i].text); //    htmldata.append(notifications->notifications[i].text);
            //      htmldata.append("</b>");
            // htmldata.append("<p><span style=\" font-size:").append(QString::number(dtFontSize)).append("pt;\">").append((QDateTime::fromTime_t(notifications->notifications[i].mtime).toString())).append("</span></p>");
            htmldata.append(dtHtmlBeginStr).append((QDateTime::fromTime_t(notifications->notifications[i].mtime).toString())).append(dtHtmlEndStr);

            notificationsModel->setData(indexHtml, QVariant(htmldata), Qt::EditRole); //displayrole

            actionsManager actionInfo; //(notifications->notifications[i].actionid,notifications->notifications[i].actiondata);
            if(notifications->notifications[i].actionid)
            {
                actionInfo.actionData = notifications->notifications[i].actiondata;
                actionInfo.actionId = notifications->notifications[i].actionid;
                //actnsMngrArr[i].actionId = notifications->notifications[i].actionid;
                //notificationsModel->setData(indexhtml, QVariant(notifications->notifications[i].actionid), (Qt::UserRole+1));

            }
            else
                actionInfo.actionId= 0;

            actnsMngrArr[i] = actionInfo;

            qDebug()<<"actnsMngrArr"<<actnsMngrArr[i].actionId<<actnsMngrArr[i].actionData.folderid;

            QModelIndex indexIcon = notificationsModel->index(i, 0, QModelIndex());
            QString iconpath = "/home/damyanka/git/psyncguiSeptm/psyncgui/images/testNtf.png";  //notifications->notifications[i].iconid
            notificationsModel->setData(indexIcon, QVariant(iconpath));

            table->openPersistentEditor(notificationsModel->index(i, 1)) ;
            table->openPersistentEditor(notificationsModel->index(i, 0)) ;
        }
*/
            free(notifications);
            notifications = NULL;

            //emit chage tray
        }
        else
            qDebug()<<"updateNotfctnsModel no new notifications";
    }
    //else
    //  updateFlag = true;

}

void NotificationsManager::showNotificationsWin()
{
    //
    //  notifywin->setFocus((Qt::FocusReason)(Qt::MouseFocusReason | Qt::TabFocusReason));
    // notifywin->show();

    //if !num - set dflt text, hide table
    notifywin->raise();
    notifywin->activateWindow();
    notifywin->showNormal();
    QPoint topLeft = app->calcWinNextToTrayCoords(notifywin->width(), notifywin->height());
    notifywin->move(topLeft);
    table->scrollToTop();

    //notifywin->setWindowState(Qt::WindowActive);
    //a.setActiveWindow(notifywin);
}

void NotificationsManager::resetNums()
{
    notifyDelegate->setNumNew(0);
    this->lastNtfctId = -1;
    cntrWid->setNumNew(0);
}

void NotificationsManager::actionExecSlot(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    switch(actnsMngrArr[index.row()].actionId)
    {
    case PNOTIFICATION_ACTION_NONE: //0
        return;
    case PNOTIFICATION_ACTION_GO_TO_FOLDER: //1
    {
        char *path = psync_fs_get_path_by_folderid(actnsMngrArr[index.row()].actionData.folderid);
        if(path != NULL)
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            free(path);
            notifywin->close();
        }
        break;
    }
    default:
        break;
    }
}

void NotificationsManager::setWinFocus()
{
    //qDebug()<<"setWinFocus";
    // notifywin->setFocus((Qt::FocusReason)(Qt::MouseFocusReason | Qt::TabFocusReason));
}

quint32 NotificationsManager::getLastNtfctId()
{
    return this->lastNtfctId;
}

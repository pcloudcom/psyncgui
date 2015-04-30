#include "notificationsmanager.h"
#include "pcloudapp.h"

#include <QDebug>
#include <QCursor>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QDesktopWidget>

NotificationsWidget::NotificationsWidget(NotificationsManager *mngr, int height, QWidget *parent) : QWidget(parent)
{
    //setFocusPolicy(Qt::ClickFocus);
    //setFocusPolicy((Qt::FocusPolicy)(Qt::TabFocus|Qt::ClickFocus));
#ifdef Q_OS_LINUX
    this->setWindowFlags(Qt::Popup | Qt::Window);
#else
    //this->setWindowFlags(Qt::Dialog);
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedSize(QSize(356,height));
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
    if(event->type() == QEvent::WindowDeactivate || event->type() == QEvent::Hide) //25  18
    {
        qDebug()<<"eventFilter win deactivate";
        if(this->isVisible())
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


CntrWidget::CntrWidget(QFont fontVal, QWidget *parent) :QWidget(parent)
{
    this->numNew = 0;
    this->cntrFont = fontVal;
#ifdef Q_OS_WIN
    this->setMaximumSize(QSize(28,28));
    this->radius = 10;
#else
    this->setMaximumSize(QSize(32,32));
    this->radius = 12;
#endif
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
    painter.drawEllipse(QPoint(event->rect().width()/2, event->rect().height()/2), radius, radius);
    painter.setPen(Qt::white);
    painter.setFont(cntrFont);
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

    QFont cntrFontVal;
    if(app->font().pointSize() > 10)
    {
        dtFontSize = app->fontPointSize -3;
        cntrFontVal = app->smaller2pFont;
    }
    else
    {
        dtFontSize = app->fontPointSize -1;
        cntrFontVal = app->smaller1pFont;
    }

    //Ü7 - for tests
    textHtmlBeginStr = QString("<html></title><body><p style = \"margin:0px;\">");
    textHtmlEndStr = QString("</p>");
    dtHtmlBeginStr = QString("<p style = \"margin-top:8px;margin-bottom:0px;margin-left:0px;margin-right:0px;font-size:").append(QString::number(dtFontSize)).append("pt; color:#797979;\">");
    dtHtmlEndStr = QString("</p></body></html>");

    int winHeight;
#ifdef Q_OS_LINUX
    QDesktopWidget *desktop = app->desktop();
    winHeight = (desktop->availableGeometry().height()/2 > 460 ? 460 : desktop->availableGeometry().height()/2);
    fldrIconPath = (":/ntf/images/notifications/20-lin.png");
#else
    winHeight = 452;
    fldrIconPath = (":/ntf/images/notifications/20-win.png");
#endif
    notifywin = new NotificationsWidget(this,winHeight);

    layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0,0,0,8));
    hlayout = new QHBoxLayout();
    hlayout->setSpacing(0);
    hlayout->setMargin(0);
    QLabel *label = new QLabel(), *icon = new QLabel();
    cntrWid = new CntrWidget(cntrFontVal);
    label->setText("pCloud Notifications");
    if(app->font().pointSize() < 12)
        label->setFont(app->bigger2pFont);
    else
        label->setFont(app->bigger1pFont);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    icon->setPixmap(QPixmap(":/48x34/images/48x34/notify.png"));
    icon->setMaximumWidth(72);
#ifdef Q_OS_LINUX
    icon->setMaximumHeight(70);
    icon->setMinimumHeight(44);
#else
    icon->setFixedHeight(44);
#endif
    icon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    hlayout->addWidget(icon);
    hlayout->addWidget(label);
    hlayout->addWidget(cntrWid);
    hlayout->setAlignment(Qt::AlignVCenter);
    layout->addLayout(hlayout);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    noNtfctnsLabel = new QLabel("No notifications available.");
    noNtfctnsLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    noNtfctnsLabel->setMargin(50);
    noNtfctnsLabel->setMinimumHeight(notifywin->height()-80);
    noNtfctnsLabel->setVisible(false);
    layout->addWidget(noNtfctnsLabel);

    table = new QTableView();
    this->setTableProps();

    qDebug()<<table->viewport()->height()<<notifywin->height();
    notificationsModel = new QStandardItemModel(0, 2);
    table->setModel(notificationsModel);
    notifyDelegate = new NotifyDelegate(table->viewport()->geometry().width(),table);
    table->setItemDelegate(notifyDelegate);
    layout->addWidget(table);

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
    table->setStyleSheet("QTableView{background-color:#FFFFFF;}");
    table->setShowGrid(false);
    table->viewport()->setAttribute(Qt::WA_Hover);
    table->setMouseTracking(true);
    QHeaderView *headerH = table->horizontalHeader(), *headerV = table->verticalHeader();
    headerH->stretchLastSection();
    headerH->hide();
    headerV->hide();
    table->setFixedWidth(notifywin->width()-16);
#ifdef Q_OS_LINUX
    table->setMinimumHeight(notifywin->height()-80);
    table->setMaximumHeight(notifywin->height()-58);
#else
    table->setFixedHeight(394);
#endif
    //table->resize(table->size());

    qDebug()<<"setTableProps"<<table->width()<<table->height()<<notifywin->width()<<notifywin->height() <<app->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
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
        else if(notifications->notifications[i].iconid != 20)
            iconpath = QString(":/ntf/images/notifications/").append(QString::number(notifications->notifications[i].iconid) + ".png"); //OFFLINE_ICON
        else
            iconpath = fldrIconPath;
        notificationsModel->setData(indexIcon, QVariant(iconpath));

        table->openPersistentEditor(notificationsModel->index(i, 1));
        table->openPersistentEditor(notificationsModel->index(i, 0));
    }
    table->resizeRowsToContents(); //recalc sizehint
    table->resizeColumnsToContents();
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
    qDebug()<<" NotificationsManager::init";
    hasTableScrollBar = false;

    psync_notification_list_t* notifications = psync_get_notifications();
    if(notifications != NULL && notifications->notificationcnt)
    {
        this->loadModel(notifications);
        free(notifications);
        notifications = NULL;
        table->resizeColumnsToContents(); //emits delegate recals size
        table->resizeRowsToContents();
    }
    else
    {
        qDebug()<<"initmodel no notif";
        this->table->setVisible(false);
        this->noNtfctnsLabel->setVisible(true);
    }
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
    if(!app->isLogedIn() && hasTableScrollBar)
    {
        this->hasTableScrollBar = false;
        notifyDelegate->updateTextDocWidth(app->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    }
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

    if(!hasTableScrollBar) //recalc textdoc width according to having scrollbar
    {
        QModelIndex lastIndex = notificationsModel->index(notificationsModel->rowCount()-1, 1, QModelIndex());
        if (table->visualRect(lastIndex).bottomRight().ry() > table->height())
        {
            hasTableScrollBar = true;
            emit notifyDelegate->updateTextDocWidth(-app->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
            table->resizeColumnsToContents(); //calls sizehint
            table->resizeRowsToContents();
            table->viewport()->updateGeometry();
            //table->repaint();
            //table->viewport()->repaint();

            /*
            QModelIndex topLeftIndex = notificationsModel->index(0, 1, QModelIndex());
            QModelIndex bottomRigthIndex = notificationsModel->index(notificationsModel->rowCount()-1, 1, QModelIndex());
            emit notificationsModel->refresh(topLeftIndex,bottomRigthIndex);
            //emit notificationsModel->layoutChanged(); //datachanged
            for (int i = 0; i < notificationsModel->rowCount(); i++)
            {
                QModelIndex indexIcon0 = notificationsModel->index(i, 0, QModelIndex());
                QModelIndex indexHtml = notificationsModel->index(i, 1, QModelIndex());
               // table->update(indexHtml);
                qDebug()<<table->visualRect(indexIcon0);
                qDebug()<<table->visualRect(indexHtml)<<table->visualRect(indexHtml).bottomRight().ry() << table->visualRect(indexHtml).size().height();
            }*/
        }
    }

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
    case PNOTIFICATION_ACTION_SHARE_REQUEST:
    {
        qDebug()<<"PNOTIFICATION_ACTION_SHARE_REQUEST";
        app->showShares();
        break;
    }
    case PNOTIFICATION_ACTION_GO_TO_URL:
    {
        QUrl url(QString(actnsMngrArr[index.row()].actionData.url));
        QDesktopServices::openUrl(url);
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

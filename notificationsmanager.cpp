#include "notificationsmanager.h"
#include "pcloudapp.h"

#include <QDebug>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QDesktopWidget>
#include <QPushButton>

NotificationsWidget::NotificationsWidget(NotificationsManager *mngr, int height, QWidget *parent) : QWidget(parent)
{    
    setFocusPolicy((Qt::FocusPolicy)(Qt::TabFocus|Qt::ClickFocus));
#ifdef Q_OS_LINUX
    this->setWindowFlags(Qt::Popup | Qt::Window);
#else    
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle(trUtf8("pCloud Notifications"));
#endif
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedSize(QSize(356,height));
    this->mngrParent = mngr;
    this->installEventFilter(this);
}

bool NotificationsWidget::eventFilter(QObject *watched, QEvent *event)
{    
    if(event->type() == QEvent::WindowDeactivate || event->type() == QEvent::Hide)
    {
        //qDebug()<<"eventFilter win deactivate";
        if(this->isVisible())
            this->close();

        mngrParent->markRead();
    }

    QWidget::eventFilter(watched, event);
}

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
#ifdef Q_OS_WIN
    if(this->numNew < 10)
        painter.drawText(QRect(QPoint(event->rect().topLeft().rx()+1 ,event->rect().topLeft().ry()-1),
                               QSize(event->rect().width(),event->rect().height())) , Qt::AlignCenter, QString::number(this->numNew));
    else
        painter.drawText(event->rect(), Qt::AlignCenter, QString::number(this->numNew));
#else
    painter.drawText(event->rect(), Qt::AlignCenter, QString::number(this->numNew));
#endif
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
    lastNtfctId = -2;

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
    if(app->desktopEnv == "ubuntu")
    {
        QPushButton* closeBtn = new QPushButton();
        closeBtn->setText("");
        closeBtn->setMaximumSize(QSize(16,16));
        closeBtn->setIcon(QIcon(":/16x16/images/16x16/close.png"));
        closeBtn->setFlat(true);
        connect(closeBtn, SIGNAL(clicked()), notifywin, SLOT(close()));
        hlayout->addWidget(closeBtn);
    }

    hlayout->setAlignment(Qt::AlignVCenter);
    layout->addLayout(hlayout);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    noNtfctnsLabel = new QLabel("No notifications available");
    noNtfctnsLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    noNtfctnsLabel->setMargin(50);
    noNtfctnsLabel->setMinimumHeight(notifywin->height()-80);
    noNtfctnsLabel->setVisible(false);
    layout->addWidget(noNtfctnsLabel);

    table = new QTableView();
    this->setTableProps();

    notificationsModel = new QStandardItemModel(0, 2);
    table->setModel(notificationsModel);
    notifyDelegate = new NotifyDelegate(table->viewport()->geometry().width(),table);
    table->setItemDelegate(notifyDelegate);
    layout->addWidget(table);

    notifywin->setLayout(layout);
    connect(table, SIGNAL(clicked(QModelIndex)), this, SLOT(actionExecSlot(QModelIndex)));
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

    for (int i = 0; i<ntfCnt; i++)
    {
        QModelIndex indexHtml = notificationsModel->index(i, 1, QModelIndex());
        QString htmldata(textHtmlBeginStr + notifications->notifications[i].text + textHtmlEndStr);
        htmldata.append(dtHtmlBeginStr).append((QDateTime::fromTime_t(notifications->notifications[i].mtime).toString())).append(dtHtmlEndStr);
        notificationsModel->setData(indexHtml, QVariant(htmldata), Qt::EditRole);

        actnsMngrArr[i].actionId = notifications->notifications[i].actionid;
        if(actnsMngrArr[i].actionId)
            actnsMngrArr[i].actionData = notifications->notifications[i].actiondata;

        QModelIndex indexIcon = notificationsModel->index(i, 0, QModelIndex());
        QString iconpath;
        if (notifications->notifications[i].thumb != NULL)
            iconpath = notifications->notifications[i].thumb;
        else if(notifications->notifications[i].iconid != 20)
            iconpath = QString(":/ntf/images/notifications/").append(QString::number(notifications->notifications[i].iconid) + ".png");
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
    //qDebug()<<"clearModel";
    notificationsModel->removeRows(0, notificationsModel->rowCount());

    if (actnsMngrArr != NULL)
    {
        free(actnsMngrArr);
        actnsMngrArr = NULL;
    }
}

void NotificationsManager::init()
{
    //qDebug()<<" NotificationsManager::init";

    hasTableScrollBar = false;
    //table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    psync_notification_list_t* notifications = psync_get_notifications();
    if(notifications != NULL && notifications->notificationcnt)
    {
        if(!table->isVisible()) //after unlink of account with no ntf
        {
            noNtfctnsLabel->setVisible(false);
            table->setVisible(true);
        }

        this->loadModel(notifications);
        free(notifications);
        notifications = NULL;
        table->resizeColumnsToContents(); //emits delegate recals size
        table->resizeRowsToContents();
    }
    else
    {
        qDebug()<<"NotificationsManager::init no notifications";
        this->table->setVisible(false);
        this->noNtfctnsLabel->setVisible(true);
    }
}

void NotificationsManager::clear()
{
    //qDebug()<<"NotificationsManager::clear"<<hasTableScrollBar;

    if(!app->isLogedIn() && hasTableScrollBar)
    {
        this->hasTableScrollBar = false;
        //table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        notifyDelegate->updateTextDocWidth(app->style()->pixelMetric(QStyle::PM_ScrollBarExtent)+app->style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarSpacing));
    }
    this->clearModel();
    this->resetNums();
}

void NotificationsManager::updateNotfctnsModel(int newcnt)
{
    qDebug()<<"NotificationsManager updateNotfctnsModel";
    if(newcnt) // callbacked is called after marking red
    {
        psync_notification_list_t* notifications = psync_get_notifications();
        if (notifications != NULL && notifications->newnotificationcnt) //first notifications
        {
            if(!table->isVisible())
            {
                noNtfctnsLabel->setVisible(false);
                table->setVisible(true);
            }

            this->clear();
            this->loadModel(notifications);
            free(notifications);
            notifications = NULL;
        }
        else
            qDebug()<<"updateNotfctnsModel no new notifications";
    }
}

void NotificationsManager::showNotificationsWin()
{    
    notifywin->setFocus((Qt::FocusReason)(Qt::MouseFocusReason | Qt::TabFocusReason));
    //qDebug()<<"showNotificationsWin"<<hasTableScrollBar;

    if(!hasTableScrollBar) //recalc textdoc width according to having scrollbar
    {
        QModelIndex lastIndex = notificationsModel->index(notificationsModel->rowCount()-1, 1, QModelIndex());
        if (table->visualRect(lastIndex).bottomRight().ry() > table->height())
        {
            hasTableScrollBar = true;
            emit notifyDelegate->updateTextDocWidth(-app->style()->pixelMetric(QStyle::PM_ScrollBarExtent) - app->style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarSpacing));
            //table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            table->resizeColumnsToContents(); //calls sizehint
            table->resizeRowsToContents();
            table->viewport()->updateGeometry();
        }
    }

    notifywin->raise();
    notifywin->activateWindow();
    notifywin->showNormal();
    QPoint topLeft = app->calcWinNextToTrayCoords(notifywin->width(), notifywin->height());
    notifywin->move(topLeft);
    table->scrollToTop();

    //notifywin->setWindowState(Qt::WindowActive);
    // app->setActiveWindow(notifywin);
}

void NotificationsManager::markRead() //called when closing the ntf win
{
    if(this->lastNtfctId != -2)
    {
        if (psync_mark_notificaitons_read(lastNtfctId) == -1)
            app->updateTrayNtfIcon(); // no internet connection update tray manually, callback is not called when no internet
        this->resetNums();
    }
}
void NotificationsManager::resetNums()
{
    notifyDelegate->setNumNew(0);
    this->lastNtfctId = -2;
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

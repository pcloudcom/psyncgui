#include "sharespage.h"
#include "psynclib.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "unistd.h"
#include <QDesktopServices>
#include <QUrl>

SharesPage::SharesPage(PCloudWindow *w, PCloudApp *a,  QObject *parent) :
    QObject(parent)
{
    win = w;
    app = a;
    win->ui->tabWidgetShares->setTabText(0, tr("My Shares"));
    //win->ui->tabWidgetShares->setTabIcon(0, QIcon(":/images/images/myshares.png"));
    win->ui->tabWidgetShares->setTabText(1, tr("Shared With Me"));
    win->ui->tabWidgetShares->setCurrentIndex(0);

    int requestsHight = 3 * win->ui->treeMyShares->height()/5;
    win->ui->treeMyRequest->setMaximumHeight(requestsHight);
    win->ui->treeRequestsWithMe->setMaximumHeight(requestsHight);

    this->setTableProps(win->ui->treeMyShares);
    this->setTableProps(win->ui->treeMyRequest);
    this->setTableProps(win->ui->treeSharedWithMe);
    this->setTableProps(win->ui->treeRequestsWithMe);

    connect(win->ui->btnMySharesStop, SIGNAL(clicked()), this, SLOT(stopShare()));
    connect(win->ui->btnSharedWithMeStop, SIGNAL(clicked()), this, SLOT(stopShare()));
    connect(win->ui->btnMyShareMofidy, SIGNAL(clicked()), this, SLOT(modifyShare()));
    connect(win->ui->btnCancelRqst, SIGNAL(clicked()), this, SLOT(cancelRqst()));
    connect(win->ui->btnDeclineReqst, SIGNAL(clicked()), this, SLOT(cancelRqst()));
    connect(win->ui->btnAcceptRqst, SIGNAL(clicked()), this, SLOT(acceptRqst()));
    connect(win->ui->btnShareFolder, SIGNAL(clicked()), app, SLOT(addNewShare()));
    connect(win->ui->tabWidgetShares, SIGNAL(currentChanged(int)), this, SLOT(refreshTab(int))); // to del
    connect(win->ui->treeMyShares, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openSharedFldr(QTreeWidgetItem*,int)));
    connect(win->ui->treeMyRequest, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openSharedFldr(QTreeWidgetItem*,int)));
    connect(win->ui->treeSharedWithMe, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openSharedFldr(QTreeWidgetItem*,int)));
    connect(win->ui->treeRequestsWithMe, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(acceptRqst()));

    loadAll();
}

SharesPage::~SharesPage()
{  
}

void SharesPage::refreshTab(int i)
{
    if(!i)
    {
        fillSharesTable(0); // tab 0 - My Shares
        fillRequestsTable(0);
    }
    else
    {
        fillSharesTable(1); // tab 1 - Shared with me
        fillRequestsTable(1);
    }
}

void SharesPage::loadAll()
{
    fillSharesTable(0); // tab 0 - My Shares
    fillRequestsTable(0);
    fillSharesTable(1); // tab 1 - Shared with me
    fillRequestsTable(1);
}

QString SharesPage::getPermissions(quint8 perm)
{
    QString res;
    res.append("Read, ");
    if(perm & PSYNC_PERM_CREATE)
        res.append("Create, ");
    if(perm & PSYNC_PERM_MODIFY)
        res.append("Modify, ");
    if (perm & PSYNC_PERM_DELETE)
        res.append("Delete, ");

    res.remove(res.length()-2,2);
    return res;
}

void SharesPage::addSharesRow(QTreeWidget *table, QStringList data, quint64 id, quint64 fldrid, quint8 perms, int index)
{
    QTreeWidgetItem* sharedItem = new QTreeWidgetItem((QTreeWidgetItem*)0,data);
    sharedItem->setData(1,Qt::UserRole,fldrid);
    sharedItem->setData(2,Qt::UserRole,perms);
    sharedItem->setData(4,Qt::UserRole,id);
    table->insertTopLevelItem(index,sharedItem);
}

void SharesPage::fillSharesTable(bool incoming)
{    
    psync_share_list_t* shares = psync_list_shares(incoming);

    if(shares != NULL)
    {
        QTreeWidget *table;
        if(!incoming)
            table = win->ui->treeMyShares;
        else
            table = win->ui->treeSharedWithMe;
        table->clear();

        for(int i = 0; i < shares->sharecnt; i++)
        {
            QStringList data;
            data<< shares->shares[i].email << shares->shares[i].sharename
                << getPermissions(shares->shares[i].permissions)
                << QDateTime::fromTime_t(shares->shares[i].created).date().toString("dd/MM/yy");

            addSharesRow(table, data, shares->shares[i].shareid, shares->shares[i].folderid, shares->shares[i].permissions, i);
        }

        free(shares);
    }
}

void SharesPage::fillRequestsTable(bool incoming)
{
    psync_sharerequest_list_t *shares = psync_list_sharerequests(incoming);

    if(shares != NULL && shares->sharerequestcnt)
    {
        this->setRequestsVisibility(incoming, true);

        QTreeWidget *table;
        if(!incoming) //tab 0
        {
            table = win->ui->treeMyRequest;
            win->ui->widget_myrequests->setVisible(true);
        }
        else //tab 1
        {
            table = win->ui->treeRequestsWithMe;
            win->ui->widget_requesteswithme->setVisible(true);
        }
        table->clear();

        for(int i = 0; i < shares->sharerequestcnt; i++)
        {
            QStringList data;
            data<< shares->sharerequests[i].email << shares->sharerequests[i].sharename
                << getPermissions(shares->sharerequests[i].permissions)
                << QDateTime::fromTime_t(shares->sharerequests[i].created).date().toString("dd/MM/yy");
            addSharesRow(table, data, shares->sharerequests[i].sharerequestid, shares->sharerequests[i].folderid, shares->sharerequests[i].permissions, i);
        }

        free(shares);
    }
    else
        this->setRequestsVisibility(incoming, false);
}

void SharesPage::setTableProps(QTreeWidget *table)
{
#ifdef Q_OS_LINUX
    table->setMinimumWidth(600);
#else
    table->setMinimumWidth(450);
#endif
    int tableWidth = table->width();
    table->setColumnWidth(0,(int) 7* tableWidth/16); //mail
    table->setColumnWidth(1,(int) 5* tableWidth/16); //name
    table->setColumnWidth(2,(int) 3* tableWidth/16); //perms
    table->setColumnWidth(3,(int) tableWidth/16); //date
#if QT_VERSION>QT_VERSION_CHECK(5,0,0)
    table->header()->setSectionsMovable(false);
#else
    table->header()->setMovable(false);
#endif
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSortingEnabled(true);
    table->sortByColumn(0, Qt::AscendingOrder);
    table->setAlternatingRowColors(true);
}

void SharesPage::setRequestsVisibility(int incoming, bool visible)
{
    if(!incoming) // tab 0
    {
        win->ui->widget_myrequests->setVisible(visible);
        win->ui->label_noMyRqsts->setVisible(!visible);
    }
    else
    {
        win->ui->widget_requesteswithme->setVisible(visible);
        win->ui->label_noRqstsWithMe->setVisible(!visible);
    }
}

// slots
void SharesPage::openSharedFldr(QTreeWidgetItem* item, int)
{
    quint64 fldrid = item->treeWidget()->currentItem()->data(1,Qt::UserRole).toLongLong();
    char *path = psync_fs_get_path_by_folderid(fldrid);
    if(path != NULL)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        free(path);
    }
    else
    {    qDebug()<<"openSharedFldr path is empty";

        QString urlstr = ("https://my.pcloud.com/#folder=" +
                          QString::number(fldrid) + "&page=filemanager&authtoken=" + psync_get_auth_string());
        QDesktopServices::openUrl(QUrl(urlstr, QUrl::TolerantMode));
    }
}

void SharesPage::stopShare() //Stop outgoing shares - My shares tables
{
    QObject* sender = QObject::sender();
    QTreeWidget* table;
    app->noEventCallbackFlag = true;

    if(sender->objectName() == "btnMySharesStop")
        table = win->ui->treeMyShares;
    else
        table = win->ui->treeSharedWithMe;

    if(!table->currentItem())
    {
        QMessageBox::warning(NULL,"pCloud","Please select a share!");
        return;
    }
    else
    {
        if(QMessageBox::question(NULL,"pCloud","Do you really want to remove the selected share?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            QTreeWidgetItem* currentItem = table->currentItem();
            char* err= NULL;
            int res = psync_remove_share(currentItem->data(4,Qt::UserRole).toULongLong() ,&err);
            if(!res)
            {
                table->takeTopLevelItem(table->indexOfTopLevelItem(currentItem));
                delete currentItem;
            }
            else
                this->getError(res, err);

            free(err);
        }
        else
            return;
    }
}

void SharesPage::cancelRqst()
{
    QObject* sender = QObject::sender();
    QTreeWidget* table;
    QString msg;

    int incoming = 0;
    if(sender->objectName() == "btnCancelRqst")
    {
        table = win->ui->treeMyRequest;
        msg = "Do you really want to cancel the selected share request?";
    }

    else
    {
        table = win->ui->treeRequestsWithMe;
        incoming = 1;
        msg = "Do you really want to reject the selected share request?";
    }

    if(!table->currentItem())
    {
        QMessageBox::warning(NULL,"pCloud","Please select a share!");
        return;
    }
    else
    {
        QTreeWidgetItem* currentItem = table->currentItem();
        if(QMessageBox::question(NULL,"pCloud",msg,QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            char* err= NULL;
            int res;
            if(!incoming)
                res= psync_cancel_share_request(currentItem->data(4,Qt::UserRole).toULongLong() ,&err);
            else
                res= psync_decline_share_request(currentItem->data(4,Qt::UserRole).toULongLong() ,&err);
            if(res)
                this->getError(res,err);
            //   table->takeTopLevelItem(table->indexOfTopLevelItem(currentItem));
            // delete currentItem;
            // from callback

            free(err);
        }
        else
            return;
    }
}

void SharesPage::modifyShare()
{
    QTreeWidgetItem* currentItem = win->ui->treeMyShares->currentItem();
    if (!currentItem)
    {
        QMessageBox::warning(NULL,"pCloud","Please select a share!");
        return;
    }
    else
    {
        ChangePermissionsDialog dialog(currentItem->data(2,Qt::UserRole).toInt(),currentItem->text(1),currentItem->text(0));
        if(dialog.exec() == QDialog::Accepted)
        {
            quint32 newperms = dialog.getNewPermissions();
            qDebug()<<"Shares: modify share"<<newperms;
            char* err = NULL;
            int res = psync_modify_share(currentItem->data(4,Qt::UserRole).toLongLong(),newperms,&err);
            dialog.hide();
            if(!res)
            {
                currentItem->setData(2,Qt::UserRole,newperms);
                currentItem->setText(2,this->getPermissions(newperms));
            }
            else
                this->getError(res,err);
            free(err);
        }
    }
}

void SharesPage::getError(int res, char* err)
{
    if(res == -1)
        QMessageBox::information(NULL,trUtf8("pCloud"), trUtf8("No internet connection"));
    else
        QMessageBox::critical(NULL,trUtf8("pCloud"), trUtf8(err));
}

void SharesPage::acceptRqst()
{
    QTreeWidgetItem* currentItem = win->ui->treeRequestsWithMe->currentItem();
    if(!currentItem)
    {
        QMessageBox::warning(NULL,"pCloud","Please select a share!");
        return;
    }
    else
    {
        AcceptShareDialog dialog(currentItem->text(1),this->win);
        if(dialog.exec() == QDialog::Accepted)
        {
            char* err = NULL;
            QString sharename = dialog.getShareName();
            quint64 fldrid = dialog.getFldrid(), rqstid = currentItem->data(4,Qt::UserRole).toULongLong();
            qDebug()<<"shares add request" << currentItem->data(4,Qt::UserRole).toULongLong()<<fldrid<<sharename;

            int res = psync_accept_share_request(rqstid, fldrid, sharename.toUtf8(), &err);
            dialog.hide();
            if(!res)
            {
                QStringList data;
                data<< currentItem->text(0) << sharename
                    <<currentItem->text(2) << currentItem->text(3);
                sleep(1); //waits shares list to update // to move in events
                this->fillSharesTable(true); // sharedid is changed
                //    win->ui->treeRequestsWithMe->takeTopLevelItem( win->ui->treeRequestsWithMe->indexOfTopLevelItem(currentItem));
                //  delete currentItem;
            }
            else
                this->getError(res,err);
            free(err);
        }
    }
}

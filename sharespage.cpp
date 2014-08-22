#include "sharespage.h"
#include "psynclib.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"

#include "unistd.h"

SharesPage::SharesPage(PCloudWindow *w, PCloudApp *a,  QObject *parent) :
    QObject(parent)
{
    win = w;
    app = a;
    sharefolderwin = NULL;
    win->ui->tabWidgetShares->setTabText(0, tr("My Shares"));
    //win->ui->tabWidgetShares->setTabIcon(0, QIcon(":/images/images/myshares.png"));
    win->ui->tabWidgetShares->setTabText(1, tr("Shared with me"));
    win->ui->tabWidgetShares->setCurrentIndex(0);

    connect(win->ui->btnMySharesStop, SIGNAL(clicked()), this, SLOT(stopShare()));
    connect(win->ui->btnSharedWithMeStop, SIGNAL(clicked()), this, SLOT(stopShare()));
    connect(win->ui->btnMyShareMofidy, SIGNAL(clicked()), this, SLOT(modifyShare()));
    connect(win->ui->btnCancelRqst, SIGNAL(clicked()), this, SLOT(cancelRqst()));
    connect(win->ui->btnDeclineReqst, SIGNAL(clicked()), this, SLOT(cancelRqst()));
    connect(win->ui->btnAcceptRqst, SIGNAL(clicked()), this, SLOT(acceptRqst()));
    connect(win->ui->btnShareFolder, SIGNAL(clicked()), this, SLOT(shareFolder()));
    connect(win->ui->tabWidgetShares, SIGNAL(currentChanged(int)), this, SLOT(refreshTab(int)));

    loadAll();
}

SharesPage::~SharesPage()
{
    if(sharefolderwin)
        delete sharefolderwin;
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


void SharesPage::addSharesRow(QTreeWidget *table, QStringList data, quint64 id, quint8 perms, int index)
{
    QTreeWidgetItem* sharedItem = new QTreeWidgetItem((QTreeWidgetItem*)0,data);
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
                << QDateTime::fromTime_t(shares->shares[i].created).date().toString().remove(0,4);

            addSharesRow(table, data, shares->shares[i].shareid, shares->shares[i].permissions, i);
        }

        free(shares);
        this->setTableProps(table);
    }
}

void SharesPage::fillRequestsTable(bool incoming)
{
    psync_sharerequest_list_t *shares = psync_list_sharerequests(incoming);

    if(shares != NULL && shares->sharerequestcnt)
    {
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
                << QDateTime::fromTime_t(shares->sharerequests[i].created).date().toString().remove(0,4);

            addSharesRow(table, data, shares->sharerequests[i].sharerequestid, shares->sharerequests[i].permissions, i);
        }

        free(shares);
        this->setTableProps(table);
    }
    else
        this->setRequestsVisibility(incoming, false);
}

void SharesPage::setTableProps(QTreeWidget *table)
{
#ifdef Q_OS_LINUX
    table->setMinimumWidth(600);
    table->setColumnWidth(0,200);
#else
    table->setMinimumWidth(450);
#endif
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSortingEnabled(true);
    table->sortByColumn(0, Qt::AscendingOrder);
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
void SharesPage::shareFolder()
{
    if (!sharefolderwin)
        sharefolderwin = new ShareFolderWindow(win,this);
    app->showWindow(sharefolderwin);
}

void SharesPage::stopShare() //Stop outgoing shares - My shares tables
{
    QObject* sender = QObject::sender();
    QTreeWidget* table;

    if(sender->objectName() == "btnMySharesStop")
        table = win->ui->treeMyShares;
    else
        table = win->ui->treeSharedWithMe;

    if(!table->currentItem())
    {
        //QMessageBox::warning(this,"pCloud","Please select a share!");
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

    int incoming = 0;
    if(sender->objectName() == "btnCancelRqst")
        table = win->ui->treeMyRequest;
    else
    {
        table = win->ui->treeRequestsWithMe;
        incoming = 1;
    }

    if(!table->currentItem())
    {
        QMessageBox::warning(NULL,"pCloud","Please select a share!");
        return;
    }
    else
    {
        QTreeWidgetItem* currentItem = table->currentItem();
        if(QMessageBox::question(NULL,"pCloud","Do you really want to reject the selected share request?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            char* err= NULL;
            int res;
            if(!incoming)
                res= psync_cancel_share_request(currentItem->data(4,Qt::UserRole).toULongLong() ,&err);
            else
                res= psync_decline_share_request(currentItem->data(4,Qt::UserRole).toULongLong() ,&err);
            if(!res)
            {
                table->takeTopLevelItem(table->indexOfTopLevelItem(currentItem));
                delete currentItem;
                // from callback
            }
            else
                this->getError(res,err);
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
        AcceptShareDialog dialog(currentItem->text(1),win);
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
                win->ui->treeRequestsWithMe->takeTopLevelItem( win->ui->treeRequestsWithMe->indexOfTopLevelItem(currentItem));
                delete currentItem;
            }
            else
                this->getError(res,err);
            free(err);
        }
    }
}



/*

void SharesPage::acceptRequest()
{
    this->type = 1;
    if (!win->ui->treeRequestsWithMe->currentItem())
        return selectErr();
    quint64 sharerequestid = win->ui->treeRequestsWithMe->currentItem()->data(0, Qt::UserRole).toULongLong();
    DirectoryPickerDialog dir(app, win);
    dir.onlyMine=true;
    dir.showRoot=true;
    dir.setWindowTitle("Select a directory to accept share to...");
    if (dir.exec()==QDialog::Rejected || !dir.ui->dirtree->currentItem())
        return;
    quint64 folderid=dir.ui->dirtree->currentItem()->data(1, Qt::UserRole).toULongLong();
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();
    if (!(conn=app->getAPISock())){
        showError(1,"Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "acceptshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("sharerequestid", sharerequestid),
                     P_NUM("folderid", folderid));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(1,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(1,find_res(res, "error")->str);
        free(res);
        return;
    }
    free(res);
    load(1);
}


*/

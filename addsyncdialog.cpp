#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"

#include <QFileSystemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>

addSyncDialog::addSyncDialog(PCloudApp *a, PCloudWindow *w, SyncPage *sp,SuggestnsBaseWin *wlcm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addSyncDialog)
{
    ui->setupUi(this);
    app = a;
    win = w;
    syncpage = sp;
    addNewSyncsWin = wlcm;
    ui->treeSyncRemote->header()->setSortIndicatorShown(true);
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addSync()));
    connect(ui->btnNewFldrLocal, SIGNAL(clicked()), this, SLOT(newLocalFldr()));
    connect(ui->btnNewFldrRemote, SIGNAL(clicked()), this, SLOT(newRemoteFldr()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hideDialog()));
    if (addNewSyncsWin && addNewSyncsWin->getChangeItem())
    {
        ui->btnAdd->setText(trUtf8("Change"));
        this->setWindowTitle(trUtf8("Change sync"));
    }
    else
        this->setWindowTitle(trUtf8("Add new sync"));
    load();
}

addSyncDialog::~addSyncDialog()
{
    delete ui;
}

void addSyncDialog::hideDialog()
{
    if (addNewSyncsWin)
        addNewSyncsWin->setChangeItem(false);
    this->hide();
}


static QList<QTreeWidgetItem *> listRemoteFldrs(QString parentPath)
{
    QList<QTreeWidgetItem *> items;
    pfolder_list_t *res = psync_list_remote_folder_by_path(parentPath.toUtf8(),PLIST_FOLDERS);

    if (res != NULL)
    {
        for(uint i = 0; i < res->entrycnt; i++)
        {
            QString path = parentPath;
            if (parentPath != "/")
                path.append("/").append(res->entries[i].name);
            else
                path.append(res->entries[i].name);
            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(res->entries[i].name));
            item->setIcon(0,QIcon(":images/images/folder-p.png"));
            item->setData(0,Qt::UserRole, path);
            item->addChildren(listRemoteFldrs(path));
            items.append(item);
            // qDebug()<<path<< "name flrd" << name.toUtf8();
        }
    }
    free(res);
    return items;
}

void addSyncDialog::load()
{    
    //remote tree
    QList<QTreeWidgetItem *> items;
    ui->treeSyncRemote->clear();
    QString root = "/";
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(QStringList(root));
    rootItem->setIcon(0,QIcon(":images/images/folder-p.png"));
    ui->treeSyncRemote->insertTopLevelItem(0,rootItem);
    ui->treeSyncRemote->setCurrentItem(rootItem);
    items = listRemoteFldrs(root);
    rootItem->addChildren(items);
    ui->treeSyncRemote->expandItem(rootItem);
    ui->treeSyncRemote->setSortingEnabled(true);
    ui->treeSyncRemote->sortByColumn(0, Qt::AscendingOrder);


    //local tree
    model = new QFileSystemModel;
    model->setReadOnly(false);
    model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QString path = QDir::homePath();
    model->setRootPath(QDir::homePath()); //currentPath
    ui->treeSyncLocal->setModel(model);
    ui->treeSyncLocal->sortByColumn(0,Qt::AscendingOrder);
    ui->treeSyncLocal->setColumnHidden(1,true);
    ui->treeSyncLocal->setColumnHidden(2, true);
    ui->treeSyncLocal->setColumnHidden(3, true);
    ui->treeSyncLocal->setAnimated(false);
    ui->treeSyncLocal->setSortingEnabled(true);
    ui->treeSyncLocal->expandAll();
    if (this->addNewSyncsWin == NULL)
    {
        ui->treeSyncLocal->setCurrentIndex(model->index(path));
    }
    // not for case anything is selected
    else
    {
        if (addNewSyncsWin->getChangeItem())
        {
            ui->treeSyncLocal->setCurrentIndex(model->index(addNewSyncsWin->getCurrLocalPath()));
            //ui->treeSyncLocal->scrollTo(model->index(addNewSyncsWin->getCurrLocalPath()),QAbstractItemView::PositionAtCenter);
            ui->comboSyncType->setCurrentIndex(addNewSyncsWin->getCurrType()+1);


            // remote tree - add new remote Fodlers from suggestions
            if (addNewSyncsWin->newRemoteFldrsLst.length() > 0)
            {
                for (int i = 0; i < addNewSyncsWin->newRemoteFldrsLst.length(); i++)
                {
                    qDebug()<<addNewSyncsWin->newRemoteFldrsLst.at(i);
                    QTreeWidgetItem *newitem = new QTreeWidgetItem((QTreeWidgetItem*)0,QStringList(addNewSyncsWin->newRemoteFldrsLst.at(i)));
                    newitem->setIcon(0, QIcon(":images/images/folder-p.png"));
                    QString rootName = addNewSyncsWin->newRemoteFldrsLst.at(i);
                    rootName.insert(0,"/");
                    newitem->setData(0, Qt::UserRole, rootName);
                    items.append(newitem);
                    rootItem->addChild(newitem);
                }
                ui->treeSyncRemote->sortByColumn(0, Qt::AscendingOrder);
            }
            //scroll to selected item
            QString remotePath = addNewSyncsWin->getCurrRemotePath();
            QString localName = addNewSyncsWin->getCurrRemotePath().section("/", -1, 1);
            QList<QTreeWidgetItem*> res =  ui->treeSyncRemote->findItems(localName,Qt::MatchExactly  | Qt::MatchRecursive);
            qDebug()<< addNewSyncsWin->getCurrRemotePath() << localName << res.length();
            if (res.length() > 0)
            {
                for (int i = 0; i < res.length(); i++)
                {
                    QTreeWidgetItem * current = res.at(i);
                    qDebug() << " find items" << res.length()<< current->data(0,Qt::UserRole);
                    if (remotePath == (current->data(0,Qt::UserRole).toString()))
                    {
                        ui->treeSyncRemote->setCurrentItem(current);
                        ui->treeSyncRemote->scrollToItem(current);
                    }
                }
            }
            else
                qDebug()<< "find items: 0";
        }
    }

}

void addSyncDialog::addSync()
{
    QString localpath,localname, remotepath;
    int type;
    localpath = model->filePath(ui->treeSyncLocal->currentIndex());
#ifdef Q_OS_WIN
    localpath.replace("/","\\");
#endif
    localname = model->fileName(ui->treeSyncLocal->currentIndex());
    remotepath.append( ui->treeSyncRemote->currentItem()->data(0,Qt::UserRole).toString());
    type = ui->comboSyncType->currentIndex();
    if(localpath == "" || remotepath == "")
    {
        QMessageBox::warning(this, "pCloud", trUtf8("Please select both paths!"));
        return;
    }
    qDebug()<<"TO ADD NEW SYNC :   localpath = "<<localpath<< " remotepath = "<<remotepath << " type = "<<type+1<< "local folder name ="<<localname;
    if(this->addNewSyncsWin)
        addNewSyncsWin->addNewItem(localpath,remotepath,type);
    else
    {
        quint32 id = psync_add_sync_by_path(localpath.toUtf8(), remotepath.toUtf8(),type+1);
        if (id == -1)
        {
            app->check_error();
            return;
        }
        QAction *fldrAction = new QAction(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),localname,app);
        fldrAction->setProperty("path",localpath);
        connect(fldrAction,SIGNAL(triggered()),app, SLOT(openLocalDir()));
        app->addNewFolderInMenu(fldrAction);
        syncpage->load();
    }
    this->hide();

}

void addSyncDialog::showError(const QString &err)
{
    QMessageBox::information(this,"", err );
}

void addSyncDialog::newLocalFldr()
{
    QModelIndex current = ui->treeSyncLocal->currentIndex();
    QDir dir(model->filePath(current));
    QString newName = "New Sync Folder";
    if(dir.exists(newName))
    {
        int i = 1;
        while(dir.exists(newName))
        {
            i++;
            newName = "New Sync Folder(" + QString::number(i) + ")";
        }
    }
    QModelIndex newIndex =  model->mkdir(current,newName);
    ui->treeSyncLocal->scrollTo(newIndex,QAbstractItemView::PositionAtCenter);
    ui->treeSyncLocal->setCurrentIndex(newIndex);
    model->setReadOnly(false);
}

void addSyncDialog::newRemoteFldr()
{
    //to check for existing name - in welcomescreen.checkRemoteName
    // add it in the list
    char *err = NULL;
    QString dirname = QInputDialog::getText(this,
                                            tr("Create Directory"),
                                            tr("Directory name"));
    if(dirname.isEmpty())
        return;
    QString parentpath = ui->treeSyncRemote->currentItem()->data(0, Qt::UserRole).toString();
    parentpath.append("/").append(dirname);
    psync_create_remote_folder_by_path(parentpath.toUtf8(),&err);
    if (err)
    {
        QMessageBox::critical(this,"pCloud",trUtf8(err));
        return;
    }
    free(err);
    if (this->addNewSyncsWin)
    {
        addNewSyncsWin->addNewRemoteFldr(dirname);
    }
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0,QStringList(dirname));
    item->setIcon(0,QIcon(":images/images/folder-p.png"));
    item->setData(0,Qt::UserRole, parentpath);
    ui->treeSyncRemote->currentItem()->insertChild(0,item);
    ui->treeSyncRemote->setCurrentItem(item);
    ui->treeSyncRemote->scrollToItem(item);
}









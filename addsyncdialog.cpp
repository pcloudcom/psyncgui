#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"

#include <QFileSystemModel>
#include <QDebug>

addSyncDialog::addSyncDialog(PCloudApp *a, PCloudWindow *w, SyncPage *sp,WelcomeScreen *wlcm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addSyncDialog)
{
    ui->setupUi(this);
    app = a;
    win = w;
    syncpage = sp;
    welcomewin = wlcm;
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addSync()));
    connect(ui->btnNewFldrLocal, SIGNAL(clicked()), this, SLOT(newLocalFldr()));
    connect(ui->btnNewFldrRemote, SIGNAL(clicked()), this, SLOT(newRemoteFldr()));
    ui->btnNewFldrRemote->setVisible(false); //temp; till newREmoteFolder is not available
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hideDialog()));
    if (welcomewin && welcomewin->getChangeItem())
    {
        ui->btnAdd->setText(tr("Change"));
        this->setWindowTitle(tr("Change sync"));
    }
    else
        this->setWindowTitle(tr("Add new sync"));

    //ui->comboSyncType->insertItem(1,QIcon(":images/images/both.png"), "");

}

addSyncDialog::~addSyncDialog()
{
    delete ui;
}

void addSyncDialog::showEvent(QShowEvent *)
{
    load();
}
void addSyncDialog::hideDialog()
{
    if (welcomewin)
        welcomewin->setChangeItem(false);
    this->hide();
}

static QList<QTreeWidgetItem *> listRemoteFldrs(QString parentPath)
{
    QList<QTreeWidgetItem *> items;
    pfolder_list_t *res = psync_list_remote_folder_by_path(parentPath.toUtf8(),PLIST_FOLDERS);

    if (res)
    {
        for(int i = 0; i < res->entrycnt; i++)
        {
            QString path = parentPath;
            if (parentPath != "/")
                path.append("/").append(res->entries[i].name);
            else
                path.append(res->entries[i].name);
            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(res->entries[i].name));
            item->setIcon(0,QIcon(":images/images/folder-p.png"));
            item->setData(0,Qt::UserRole, path);
            //or to set fldr id
            item->addChildren(listRemoteFldrs(path));
            items.append(item);
           // qDebug()<<path;
        }
    }
    return items;
}

void addSyncDialog::load()
{
    //remote tree
    QList<QTreeWidgetItem *> items;
    QString root = "/";
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(QStringList(root));
    ui->treeSyncRemote->insertTopLevelItem(0,rootItem);
    ui->treeSyncRemote->setCurrentItem(rootItem);
    items = listRemoteFldrs(root);
    rootItem->addChildren(items);
    //add new folders
    ui->treeSyncRemote->expandItem(rootItem);
    ui->treeSyncRemote->sortByColumn(0, Qt::AscendingOrder);
    ui->treeSyncRemote->setSortingEnabled(true);

    //local tree
    model = new QFileSystemModel;
    model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QString path = QDir::homePath();
    model->setRootPath(QDir::homePath()); //currentPath
    //model->setRootPath(path); //currentPath
    ui->treeSyncLocal->setModel(model);
    ui->treeSyncLocal->setColumnHidden(1,true);
    ui->treeSyncLocal->setColumnHidden(2, true);
    ui->treeSyncLocal->setColumnHidden(3, true);
    ui->treeSyncLocal->setAnimated(false);
    // ui->treeSyncLocal->setIndentation(20);
    ui->treeSyncLocal->setSortingEnabled(true);
    ui->treeSyncLocal->expandAll();
    if (this->welcomewin == NULL)
    {
        ui->treeSyncLocal->setCurrentIndex(model->index(path));
    }
    // not for case anything is selected
    else
    {
        if (welcomewin->getChangeItem())
        {
            //
            //QFileSystemModel::directoryLoaded ( const QString & path ) [signal]
            //  emit model->directoryLoaded(welcomewin->getCurrLocalPath());
            //ui->treeSyncLocal->scrollTo(model->index(welcomewin->getCurrLocalPath()));

            ui->treeSyncLocal->scrollTo(model->index(welcomewin->getCurrLocalPath()),QAbstractItemView::PositionAtCenter);
            ui->treeSyncLocal->setCurrentIndex(model->index(welcomewin->getCurrLocalPath()));

            //ui->treeSyncRemote->setCurrentI;

            //QModelIndex index = model->index(QDir::currentPath());
            //ui->treeSyncLocal->expand(index);
            //ui->treeSyncLocal->scrollTo(index);
            //ui->treeSyncLocal->scrollTo(ui->treeSyncLocal->currentIndex(),QAbstractItemView::PositionAtCenter); //QAbstractItemView::PositionAtCenter

            ui->comboSyncType->setCurrentIndex(welcomewin->getCurrType());
            // welcomewin->setChangeItem(false);


            // for test:
            QList<QTreeWidgetItem*> res =  ui->treeSyncRemote->findItems(welcomewin->getCurrRemotePath(),Qt::MatchContains  | Qt::MatchRecursive);
            if (res.length() > 0)
            {
                for (int i = 0; i < res.length(); i++)
                qDebug() << " find items" << res.length()<< res.at(i)->data(0,Qt::UserRole);
            }

        }
    }

}

void addSyncDialog::addSync()
{
    QString localpath,localname, remotepath;
    int type;
    localpath = model->filePath(ui->treeSyncLocal->currentIndex());
    localname = model->fileName(ui->treeSyncLocal->currentIndex());
    remotepath = "/";
    remotepath.append( ui->treeSyncRemote->currentItem()->data(0,0).toString());
    type = ui->comboSyncType->currentIndex();
    qDebug()<<"TO ADD NEW SYNC :   localpath = "<<localpath<< " remotepath = "<<remotepath << " type = "<<type<< "local folder name ="<<localname;
    if(this->welcomewin)
        welcomewin->addNewItem(localpath,remotepath,type);
    else
    {
        psync_add_sync_by_path(localpath.toUtf8(), remotepath.toUtf8(),type+1);
        syncpage->load();
    }
    this->hide();
    QAction *fldrAction = new QAction(localname,app);
    fldrAction->setProperty("path",localpath);
    connect(fldrAction,SIGNAL(triggered()),app, SLOT(openLocalDir()));
    app->addNewFolderInMenu(fldrAction);

}

void addSyncDialog::showError(const QString &err)
{
    QMessageBox::information(this,"", err );
}

void addSyncDialog::newLocalFldr()
{
    QModelIndex current = ui->treeSyncLocal->currentIndex();
    QDir dir(model->filePath(current));
    QString newName = "New Folder";
    if(dir.exists(newName))
    {
        int i = 1;
        while(dir.exists(newName))
        {
            i++;
            newName = "New Folder(" + QString::number(i) + ")"; // here to take default str for new folder
        }
    }
    model->mkdir(current,newName);
    model->setReadOnly(false);
    //++ to scroll to it
}

void addSyncDialog::newRemoteFldr()
{
    //to check for existing name - in welcomescreen.checkRemoteName
    // add it in the list
}









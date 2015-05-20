#include "remotetreesdialog.h"
#include "ui_remotetreesdialog.h"
#include "pcloudwindow.h"
#include <QInputDialog>

psync_folderid_t cryptoFldrId;
QIcon pFldrIcon;

RemoteTreesDialog::RemoteTreesDialog(QString curritem, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteTreesDialog)
{
    ui->setupUi(this);
    pFldrIcon.addPixmap(QPixmap(":/16x16/images/16x16/folder-p.png"), QIcon::Normal);
    pFldrIcon.addPixmap(QPixmap(":/16x16/images/16x16/folder-p-w.png"), QIcon::Selected);
    cryptoFldrId = psync_crypto_folderid();
    qDebug()<<"RemoteTreesDialog"<<cryptoFldrId;
    if (parent != NULL)
        this->setParent(parent);       
    if(!curritem.isNull())
        this->currentItemPath = curritem;
    ui->widget_fldrName->setVisible(false); /// for acceptshare - to edin fldr name   
    this->init();
    connect(ui->btnAccept, SIGNAL(clicked()), this,SLOT(setSelectedFolder()));
    connect(ui->btnReject, SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->btnNewFolder, SIGNAL(clicked()), this, SLOT(newRemoteFldr()));
    this->setWindowIcon(QIcon(WINDOW_ICON));
    this->setWindowTitle("Choose pCloud Drive Folder");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

static QList<QTreeWidgetItem *> listRemoteFldrs(QString parentPath)
{
    QList<QTreeWidgetItem *> items;
    pfolder_list_t *res = psync_list_remote_folder_by_path(parentPath.toUtf8(),PLIST_FOLDERS);

    if (res != NULL)
    {
        for(uint i = 0; i < res->entrycnt; i++)
        {
            if(cryptoFldrId != PSYNC_CRYPTO_INVALID_FOLDERID && res->entries[i].folder.folderid == cryptoFldrId)
                continue;

            QString path = parentPath;
            if (parentPath != "/")
                path.append("/").append(res->entries[i].name);
            else
                path.append(res->entries[i].name);

            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(res->entries[i].name));
            item->setIcon(0, pFldrIcon);
            item->setData(0, Qt::UserRole, path);
            item->setData(1, Qt::UserRole, (quint64)res->entries[i].folder.folderid);
            item->addChildren(listRemoteFldrs(path));
            items.append(item);
        }
    }

    free(res);
    return items;
}

void RemoteTreesDialog::init()
{
    if(ui->treeRemoteFldrs->topLevelItemCount())
        ui->treeRemoteFldrs->clear();    

    QList<QTreeWidgetItem *> items;   
    ui->treeRemoteFldrs->setColumnCount(1);
    ui->treeRemoteFldrs->setHeaderLabels(QStringList("Name"));
    QString root = "/";
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(QStringList(root));  // ??
    rootItem->setIcon(0, pFldrIcon);
    rootItem->setData(0, Qt::UserRole,root); //set path
    rootItem->setData(1,Qt::UserRole,0); //id
    ui->treeRemoteFldrs->insertTopLevelItem(0,rootItem);
    ui->treeRemoteFldrs->setCurrentItem(rootItem);
    this->root = rootItem;
    items = listRemoteFldrs(root);
    rootItem->addChildren(items);
    ui->treeRemoteFldrs->expandItem(rootItem);
    ui->treeRemoteFldrs->setSortingEnabled(true);
    ui->treeRemoteFldrs->sortByColumn(0, Qt::AscendingOrder);

}

RemoteTreesDialog::~RemoteTreesDialog()
{
    delete ui;
}

void RemoteTreesDialog::showEvent(QShowEvent *event)
{
    ui->treeRemoteFldrs->setCurrentItem(this->root);
    event->accept();
}

QString RemoteTreesDialog::newRemoteFldr()
{  
    QString dirname = QInputDialog::getText(this, tr("Create New Folder"), tr("Folder name"));
    if(dirname.isEmpty())
        return "";

    QString parentpath = ui->treeRemoteFldrs->currentItem()->data(0, Qt::UserRole).toString();
    if(parentpath != "/")
        parentpath.append("/");
    parentpath.append(dirname);

    char *err = NULL;
    psync_create_remote_folder_by_path(parentpath.toUtf8(),&err);
    if (err)
    {
        QMessageBox::critical(this,"pCloud",trUtf8(err));
        return "";
    }
    free(err);

    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem*)0,QStringList(dirname));
    item->setIcon(0, pFldrIcon);
    item->setData(0,Qt::UserRole, parentpath);
    ui->treeRemoteFldrs->currentItem()->insertChild(0,item);
    ui->treeRemoteFldrs->setCurrentItem(item);
    ui->treeRemoteFldrs->scrollToItem(item);

   if(this->parent() != NULL && this->parent()->objectName() == "addSyncDialog")
   {       
       QObject *obj = this->parent();       
       addSyncDialog *parent = qobject_cast<addSyncDialog*>(obj);
       parent->newRemoteFldr(dirname);
   }

    return dirname;
}

void RemoteTreesDialog::setSelectedFolder()
{    
    if(!ui->treeRemoteFldrs->currentItem())
    {
        QMessageBox::warning(this,"pCloud",trUtf8("No remote folder is selected. Please click on a folder to select"));
        return;
    }
    else
    {
        fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
        fldrPath = ui->treeRemoteFldrs->currentItem()->data(0,Qt::UserRole).toString();
        this->accept();
    }
}

quint64 RemoteTreesDialog::getFldrid()
{
    return this->fldrid;
}
QString RemoteTreesDialog::getFldrPath()
{
    return this->fldrPath;
}

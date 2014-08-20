#include "acceptsharedialog.h"
#include "ui_acceptsharedialog.h"

AcceptShareDialog::AcceptShareDialog(const char* name,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AcceptShareDialog)
{
    ui->setupUi(this);
    sharename = name;
    ui->line_folderName->setText(QString(name));
    this->initTree();

    connect(ui->btnAccept, SIGNAL(clicked()), this,SLOT(addRequest()));
    connect(ui->btnReject, SIGNAL(clicked()),this,SLOT(hide()));

    this->setWindowIcon(QIcon(WINDOW_ICON));
    this->setWindowTitle("pCloud");
}

AcceptShareDialog::~AcceptShareDialog()
{
    delete ui;
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
            item->setIcon(0, QIcon(":images/images/folder-p.png"));
            item->setData(0, Qt::UserRole, path);
            item->setData(1, Qt::UserRole, (quint64)res->entries[i].folder.folderid);
            item->addChildren(listRemoteFldrs(path));
            items.append(item);
        }
    }

    free(res);
    return items;
}

void AcceptShareDialog::initTree()
{
    //pcloud folders
    QList<QTreeWidgetItem *> items;
    ui->treeRemoteFldrs->clear();
    QString root = "/";
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(QStringList(root));
    rootItem->setIcon(0,QIcon(":images/images/folder-p.png"));
    rootItem->setData(1,Qt::UserRole,0);
    ui->treeRemoteFldrs->insertTopLevelItem(0,rootItem);
    ui->treeRemoteFldrs->setCurrentItem(rootItem);
    items = listRemoteFldrs(root);
    rootItem->addChildren(items);
    ui->treeRemoteFldrs->expandItem(rootItem);
    ui->treeRemoteFldrs->setSortingEnabled(true);
    ui->treeRemoteFldrs->sortByColumn(0, Qt::AscendingOrder);
}

void AcceptShareDialog::addRequest()
{
    fldrid = ui->treeRemoteFldrs->currentItem()->data(1,Qt::UserRole).toULongLong();
    if(ui->line_folderName->text().compare(sharename))
        sharename = ui->line_folderName->text().toUtf8().constData();
    else
        sharename = NULL;

    this->accept();
}

quint64 AcceptShareDialog::getFldrid()
{
    return this->fldrid;
}

const char* AcceptShareDialog::getShareName()
{
    return this->sharename;
}

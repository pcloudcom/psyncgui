#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include "pcloudapp.h"
#include "addsyncdialog.h"
#include "modifysyncdialog.h"
#include "ui_modifysyncdialog.h"
#include "psynclib.h"
#include "common.h"

#include <QDesktopServices>
#include <QDebug>
#include <QComboBox>
#include <QStandardItemModel>

const char* typeStr[3]={"Donwload only", "Upload only", "Download and Upload"};

WelcomeScreen::WelcomeScreen(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WelcomeScreen)
{
    app = a;
    ui->setupUi(this);
    isChangingItem = false;
    connect(ui->btnAdd, SIGNAL(clicked()),this, SLOT(addSync()));
    connect(ui->btnFinish, SIGNAL(clicked()), this, SLOT(finish()));
    connect(ui->treeWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeCurrItem(QModelIndex)));
    connect(ui->btnModify, SIGNAL(clicked()), this, SLOT(modifyType()));
    //++ slot za currentdata

    this->setWindowTitle("Welcome to pCloud");
    this->setWindowIcon(QIcon(WINDOW_ICON));
    //default synced fldrs
    QString root = "/";
    pfolder_list_t *remoteRootChildFldrs = psync_list_remote_folder_by_path(root.toUtf8(),PLIST_FOLDERS);
    //build list with remote root child names in order to check: when we add new sync delay if the folderName exists.
    for (int i = 0; i < remoteRootChildFldrs->entrycnt; i++)
        remoteFldrsNamesLst.append(remoteRootChildFldrs->entries[i].name);
    qDebug()<<"Remote init folders list form lib"<<remoteFldrsNamesLst;
    QString defaultRemoteFldr = checkRemoteName("pCloudSync");
    defaultRemoteFldr.insert(0,"/");

    QTreeWidgetItem *defaultItem = new QTreeWidgetItem(ui->treeWidget); // the default sync; the first item in the view; uneditable
    defaultItem->setCheckState(0,Qt::Checked);
    defaultItem->setFlags(Qt::NoItemFlags);


    QString path = QDir::home().path().append("/pCloudSync");
    QDir pcloudDir(path);
    QString nativepath;
#ifdef Q_OS_WIN
    nativepath = pcloudDir.toNativeSeparators(path);
#else
    nativepath = pcloudDir.path();
#endif
    //QDir pcloudDir(path.append("pCloudSync"));
    if(!pcloudDir.exists())
    {
        QDir::home().mkdir("pCloudSync");
        remoteFldrsNamesLst.append("pCloudSync");
        newRemoteFldrsLst.append("pCloudSync");
    }
    //home.mkdir("pCloudSync");
    //QString
    //if(QDir(path.append("/pCloudSync")))
    //    QDir::home().mkdir("pCloudSync");
    // QDir defaultLocaldir;

    defaultItem->setText(1,nativepath);
    defaultItem->setData(1,Qt::UserRole, nativepath);
    defaultItem->setText(2,trUtf8("Download and Upload"));
    //defaultItem->setIcon(2,QIcon(":images/images/both.png"));
    defaultItem->setData(2,Qt::UserRole, PSYNC_FULL -1); //for combos and typestr[] indexes
    defaultItem->setText(3,defaultRemoteFldr);
    defaultItem->setData(3,Qt::UserRole,defaultRemoteFldr);
    ui->treeWidget->insertTopLevelItem(0,defaultItem);

    psuggested_folders_t *suggestedFldrs = psync_get_sync_suggestions();
    for (int i = 0; i < suggestedFldrs->entrycnt; i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        //item->setCheckState(0,Qt::Checked);
        item->setCheckState(0,Qt::Unchecked);
        item->setData(0,Qt::UserRole,true);
        item->setText(1,suggestedFldrs->entries[i].localpath);
        item->setData(1,Qt::UserRole,suggestedFldrs->entries[i].localpath);
        QString rootName = checkRemoteName(suggestedFldrs->entries[i].name);
        remoteFldrsNamesLst.append(rootName);
        newRemoteFldrsLst.append(rootName);
        //root.append(rootName);
        rootName.insert(0,"/");
        item->setText(3,rootName);
        item->setData(3,Qt::UserRole,rootName);
        item->setText(2,typeStr[2]);
        item->setData(2,Qt::UserRole,PSYNC_FULL -1 );
    }

    //  ui->treeWidget->sortByColumn(1,Qt::AscendingOrder);

    ui->treeWidget->resizeColumnToContents(0);
    //ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->setColumnWidth(1,250);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
    ui->treeWidget->setMinimumWidth(500);
    // ui->tableWidget->setItemDelegate(new SyncItemsDelegate());
}

void WelcomeScreen::testPrintTree(QTreeWidgetItem* itm,int col)
{
    qDebug()<< ui->treeWidget->currentItem()<< " "<< ui->treeWidget->currentColumn() << " " <<itm << " "<<col;

}

void WelcomeScreen::testPrint(QModelIndex index)
{
    qDebug()<<index;
    //qDebug()<<ui->tableWidget->currentIndex();
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}
void WelcomeScreen::closeEvent(QCloseEvent *event)
{
    //check is default sync created(finishes pressed)
    this->hide();
    event->ignore();
}
void WelcomeScreen::addSync()
{
    addSyncDialog *addDialog = new addSyncDialog(app,app->pCloudWin, app->pCloudWin->get_sync_page(),this);
    if (this->isChangingItem)
    {
        currentLocal = ui->treeWidget->currentItem()->data(1,Qt::UserRole).toString();
        currentType = ui->treeWidget->currentItem()->data(2,Qt::UserRole).toInt();
        currentRemote = ui->treeWidget->currentItem()->data(3,Qt::UserRole).toString();
        // this->isChangingItem = false;
    }
    addDialog->exec();
    //this->load();

}
//when user double-clicks on an item
void WelcomeScreen::changeCurrItem(QModelIndex index)
{
    if(index.row()) // the first item in the treeview is default and shoudn't be modified
    {
        this->isChangingItem = true;
        emit this->addSync();
    }
}
bool WelcomeScreen::getChangeItem()
{
    return this->isChangingItem;
}

void WelcomeScreen::load()
{

}
void WelcomeScreen::addNewItem(QString localpath, QString remotepath, int type)
{
    QTreeWidgetItem *item;
    if(isChangingItem)
    {
        isChangingItem = false;
        item = ui->treeWidget->currentItem();
    }
    else
        item = new QTreeWidgetItem(ui->treeWidget);

    item->setCheckState(0,Qt::Checked);
    item->setData(0,Qt::UserRole,true);
    item->setText(1,localpath);
    item->setData(1,Qt::UserRole,localpath);
    item->setText(3,remotepath);
    item->setData(3,Qt::UserRole,remotepath); //da e remote path
    item->setText(2,typeStr[type]);
    item->setData(2,Qt::UserRole,type);
}

void WelcomeScreen::modifyType()
{
    QTreeWidgetItem *current = ui->treeWidget->currentItem();
    if(!current)
    {
        QMessageBox::information(this,"",trUtf8("Please select an item to modify"));
        return;
    }
    else
    {
        ModifySyncDialog dialog(current->data(1,Qt::UserRole).toString(),current->data(3,Qt::UserRole).toString(), current->data(2,Qt::UserRole).toInt());
        if (dialog.exec() == QDialog::Accepted)
        {
            int newType = dialog.returnNewType();
            current->setData(2,Qt::UserRole, newType);
            dialog.hide();
            //this->load();
            current->setText(2,typeStr[newType]);
            // ++ modify sync type // type++
        }
    }
}
void WelcomeScreen::finish()
{
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it)
    {
        if ((*it)->checkState(0) == Qt::Checked)
        {
            QString localpath = (*it)->data(1,Qt::UserRole).toString();
            qDebug()<<"ADDING item delayed (from suggestions) " << (*it)->data(1,Qt::UserRole).toString().toUtf8() << (*it)->data(3,Qt::UserRole).toString().toUtf8() <<((*it)->data(2,Qt::UserRole).toInt() +1);
            psync_add_sync_by_path_delayed(localpath.toUtf8(),
                                           (*it)->data(3,Qt::UserRole).toString().toUtf8(),
                                           ((*it)->data(2,Qt::UserRole).toInt()) +1) ;

            QString name = localpath.section("/", -1);
            QAction * fldrAction = new QAction(name, app);
            fldrAction->setProperty("path", localpath);
            connect(fldrAction, SIGNAL(triggered()), app, SLOT(openLocalDir()));
            app->addNewFolderInMenu(fldrAction);
        }
        ++it;
    }
    app->pCloudWin->get_sync_page()->load();
    //refresh menu
    this->hide();
}
QString WelcomeScreen::checkRemoteName(QString entryName)
{
    if (remoteFldrsNamesLst.contains(entryName))
    {
        int i = 1;
        QString newName = entryName;
        while (remoteFldrsNamesLst.contains(newName)) {
            newName = entryName + "(" + QString::number(i) + ")";
            i++;
        }
        return newName;
    }
    else
        return entryName;
}
void WelcomeScreen::addNewRemoteFldr(QString name)
{
    this->remoteFldrsNamesLst.append(name);
}

QString WelcomeScreen::getCurrLocalPath()
{
    return this->currentLocal;
}
QString WelcomeScreen::getCurrRemotePath()
{
    return this->currentRemote;
}
int WelcomeScreen::getCurrType()
{
    return this->currentType;
}
QTreeWidgetItem* WelcomeScreen::getCurrItem()
{
    return ui->treeWidget->currentItem();
}
void WelcomeScreen::setChangeItem(bool b)
{
    this->isChangingItem = b;
}


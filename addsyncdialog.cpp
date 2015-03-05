#include "addsyncdialog.h"
#include "ui_addsyncdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>

addSyncDialog::addSyncDialog(PCloudApp *a, PCloudWindow *w, SuggestnsBaseWin *wlcm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addSyncDialog)
{
    ui->setupUi(this);
    app = a;
    win = w;
    addNewSyncsWin = wlcm;
    if (addNewSyncsWin != NULL && addNewSyncsWin->getChangeItem())
        remotesDialog = new RemoteTreesDialog(addNewSyncsWin->getCurrRemotePath(), this); //dialog is for changing a suggested sync
    else
        remotesDialog = new RemoteTreesDialog("", this); // dialog is for selecting new sync
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addSync()));
    connect(ui->btnOpenLocals, SIGNAL(clicked()), this, SLOT(chooseLocalFldr()));
    connect(ui->btnOpenRemotes, SIGNAL(clicked()), this, SLOT(chooseRemoteFldr()));
    //connect(ui->btnNewFldrLocal, SIGNAL(clicked()), this, SLOT(newLocalFldr()));
    //connect(ui->btnNewFldrRemote, SIGNAL(clicked()), this, SLOT(newRemoteFldr()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hideDialog()));
    if (addNewSyncsWin && addNewSyncsWin->getChangeItem())
    {
        ui->btnAdd->setText(trUtf8("Change"));
        this->setWindowTitle(trUtf8("Change sync"));
        localpath = addNewSyncsWin->getCurrLocalPath();
        ui->btnOpenLocals->setText(getDisplFldrPath(localpath,'\\'));
        remotepath = addNewSyncsWin->getCurrRemotePath();
        ui->btnOpenRemotes->setText(getDisplFldrPath(remotepath, '/'));
    }
    else
    {
        this->setWindowTitle(trUtf8("Add New Sync"));
        localpath = "";
        remotepath = "";
    }
    // load();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(300, this->height());
}

addSyncDialog::~addSyncDialog()
{
    delete ui;
    delete remotesDialog;
}

void addSyncDialog::hideDialog()
{
    if (addNewSyncsWin)
        addNewSyncsWin->setChangeItem(false);
    this->hide();
}

QString addSyncDialog::getDisplFldrPath(QString fldrpath, QChar osSep)
{
    if(fldrpath.isEmpty())
        return "";

    if(fldrpath.length() < 40)
        return fldrpath;
    else
    {
        int namelen = fldrpath.section(osSep,-1).length();
        if(namelen < 34)
            return fldrpath.left(40 - namelen).append("...").append(fldrpath.right(namelen));
        else
            return fldrpath.left(3).append("...").append(fldrpath.right(36)); //c:\...last 36
    }
}

void addSyncDialog::chooseLocalFldr()
{
    QString dfltlocalpath;
    if (localpath == "")
    {
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        dfltlocalpath = QDir::homePath(); //lin
#else
        dfltlocalpath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); //windows
#endif
    }
    else
        dfltlocalpath = localpath;
    QFileDialog* d = new QFileDialog(this);
    d->setWindowIcon(QIcon(WINDOW_ICON));
    d->setFileMode(QFileDialog::DirectoryOnly);
    this->localpath = d->getExistingDirectory(this,"Choose Local Directory", dfltlocalpath, QFileDialog::ShowDirsOnly
                                              | QFileDialog::DontResolveSymlinks);
    if (this->localpath.isEmpty())
        return;
#ifdef Q_OS_WIN
    localpath.replace("/","\\");
    ui->btnOpenLocals->setText(getDisplFldrPath(localpath, '\\'));
#else
    ui->btnOpenLocals->setText(getDisplFldrPath(localpath, '/'));
#endif
    qDebug()<<"addSyncDialog::chooseLocalFldr"<<localpath;

    /*
    if(d->exec() == QFileDialog::Accepted)
    {
        QStringList filenames = d->selectedFiles();
        if (filenames.isEmpty())
        {
            QMessageBox::critical(this, "pCloud", trUtf8("Please select local folder!"));
            return;
        }

        this->localpath = filenames.at(0);
       // ui->btnOpenLocals->setText(getDisplFldrPath(localpath.section("/",-1)));

        for (int i = 0; i < filenames.size(); i++)
            qDebug()<<"FIledialog"<< filenames.at(i);
    }
    else
    {
        this->localpath = "";
        qDebug()<<"add local fodler for new sync CANCEDL";
    }
    */
}

void addSyncDialog::chooseRemoteFldr()
{
    this->remotesDialog->exec();
    this->remotepath = remotesDialog->getFldrPath();
    if (!this->remotepath.isNull())
        ui->btnOpenRemotes->setText(getDisplFldrPath(remotepath,'/'));
}

void addSyncDialog::load() //obsolete
{    
    //remote tree
    //  win->initRemoteTree(ui->treeSyncRemote);

    /*   //local tree
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
    {      if (addNewSyncsWin->getChangeItem())
        {
            ui->treeSyncLocal->setCurrentIndex(model->index(addNewSyncsWin->getCurrLocalPath()));
            //ui->treeSyncLocal->scrollTo(model->index(addNewSyncsWin->getCurrLocalPath()),QAbstractItemView::PositionAtCenter);
            ui->comboSyncType->setCurrentIndex(addNewSyncsWin->getCurrType()-1);
        }
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
                QTreeWidgetItem* rootItem = ui->treeSyncRemote->topLevelItem(0);
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
*/
}

void addSyncDialog::addSync()
{
    int type = PSYNC_FULL - 1; // no one-way sync anymore
    /*
    QString localpath,localname, remotepath;

    localpath = model->filePath(ui->treeSyncLocal->currentIndex());
#ifdef Q_OS_WIN
    localpath.replace("/","\\");
#endif
    localname = model->fileName(ui->treeSyncLocal->currentIndex());
    remotepath.append( ui->treeSyncRemote->currentItem()->data(0,Qt::UserRole).toString());
    //type = ui->comboSyncType->currentIndex();
    */

    if(this->localpath == "" || this->remotepath == "")
    {
        QMessageBox::warning(this, "pCloud Drive", trUtf8("Please select both paths!"));
        return;
    }

#ifdef Q_OS_WIN
    if (this->localpath.startsWith("\\\\pCloud\\pCloud Drive",Qt::CaseInsensitive))
    {
        QMessageBox::critical(this, "pCloud Drive", trUtf8("Can not add new sync: The selected local folder is from pDrive!\nPlease select correct local folder!"));
        return;
    }
#endif

    QString localname;
    if(localpath != "/")
        localname = localpath.section("/",-1);
    else
        localname = localpath;

    //qDebug()<<"TO ADD NEW SYNC :   localpath = "<<localpath<< " remotepath = "<<remotepath << " type = "<<type+1<< "local folder name ="<<localname;
    qDebug()<<"TO ADD NEW SYNC :   localpath = "<<localpath<< " remotepath = "<<remotepath << "local folder name ="<<localname;

    if(this->addNewSyncsWin)
        addNewSyncsWin->addNewItem(localpath,remotepath,type);
    else
    {
        quint32 id = psync_add_sync_by_path(localpath.toUtf8(), remotepath.toUtf8(), PSYNC_FULL);
        if (id == -1)
        {
            qDebug()<<"err adding new sync"<<psync_get_last_error();
            app->check_error();
            return;
        }
        QAction *fldrAction = new QAction(QIcon(":/menu/images/menu 48x48/emptyfolder.png"),localname,app);
        fldrAction->setProperty("path",localpath);
        connect(fldrAction,SIGNAL(triggered()),app, SLOT(openLocalDir()));
        app->addNewFolderInMenu(fldrAction);
        //syncpage->load();
        win->get_sync_page()->load();
    }
    this->hide();

}

void addSyncDialog::showError(const QString &err)
{
    QMessageBox::information(this,"", err );
}

void addSyncDialog::newRemoteFldr(QString dirname)
{
    qDebug()<<"addSyncDialog"<<dirname;
    if (this->addNewSyncsWin && dirname != "")
    {
        addNewSyncsWin->addNewRemoteFldr(dirname);
    }
}

/*obsolete - old specifications
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
*/

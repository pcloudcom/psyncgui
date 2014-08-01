#include "welcomewin.h"
//#include "common.h"
#include "psynclib.h"
#include <QDesktopServices>
#include <QDir>

WelcomeWin::WelcomeWin(PCloudApp *a,QWidget *parent) :
    SuggestnsBaseWin(a,NULL,parent)
{
//initFldrs = null
    QString defaultRemoteFldr = "/pCloudSync";
    QTreeWidgetItem *defaultItem = new QTreeWidgetItem(ui->treeWidget); // the default sync; the first item in the view; uneditable
    defaultItem->setCheckState(0,Qt::Checked);
    defaultItem->setFlags(Qt::NoItemFlags);

    QString path = QDir::home().path().append("/pCloudSync");
    QDir pcloudDir(path);
    QList<QStringList> itemsLst;
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDir docs (QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
    QDir music(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    QDir photos(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    QDir movies(QDesktopServices::storageLocation(QDesktopServices::MoviesLocation));
#else
    QDir docs(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QDir music(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    QDir photos(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    QDir movies(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
#endif

    QString nativepath;
#ifdef Q_OS_WIN
    QString docspath;
    nativepath = pcloudDir.toNativeSeparators(path);
    docspath = docs.toNativeSeparators(docs.absolutePath());
    itemsLst << (QStringList() << docspath << "/My Documents");
    // in win xp My music, My pictures and My videos are located in My documents by default
    if (QSysInfo::windowsVersion() != QSysInfo::WV_XP)
    {
        QString musicpath,moviespath, photospath;
        musicpath = music.toNativeSeparators(music.absolutePath());
        moviespath = movies.toNativeSeparators(music.absolutePath());
        photospath = photos.toNativeSeparators(photos.absolutePath());
        itemsLst << (QStringList() << musicpath << "/My Music")
                 << (QStringList() << photospath << "/My Pictures")
                 << (QStringList() << moviespath << "/My Videos");
    }
#else
    nativepath = pcloudDir.path();
    itemsLst << (QStringList() << docs.absolutePath() << "/My Documents")
             << (QStringList() << music.absolutePath() << "/My Music")
             << (QStringList() << photos.absolutePath() << "/My Pictures")
             << (QStringList() << movies.absolutePath() << "/My Videos");
#endif
    if(!pcloudDir.exists())
    {
        QDir::home().mkdir("pCloudSync");
        remoteFldrsNamesLst.append("pCloudSync");
        newRemoteFldrsLst.append("pCloudSync");
    }

    defaultItem->setText(1,nativepath);
    defaultItem->setData(1,Qt::UserRole, nativepath);
    defaultItem->setText(2,trUtf8("Download and Upload"));
    defaultItem->setData(2,Qt::UserRole, PSYNC_FULL -1); //for combos and typestr[] indexes
    defaultItem->setText(3,defaultRemoteFldr);
    defaultItem->setData(3,Qt::UserRole,defaultRemoteFldr);
    ui->treeWidget->insertTopLevelItem(0,defaultItem);

    remoteFldrsNamesLst.append("My Documents");
    newRemoteFldrsLst.append("My Documents");

    //temp addInitItems(itemsLst);

    // to add them here ++ checkRemoteName !
    /* for(int i = 0; i < itemsLst.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setCheckState(0,Qt::Unchecked);
        item->setData(0,Qt::UserRole,false);
        item->setText(1,itemsLst.at(i).at(0));
        item->setData(1, Qt::UserRole,itemsLst.at(i).at(0));
        item->setText(3, itemsLst.at(i).at(1));
        item->setData(3, Qt::UserRole, itemsLst.at(i).at(1));
        item->setText(2, typeStr[2]);
        item->setData(2, Qt::UserRole, PSYNC_FULL - 1);
    }
    */

    /*  psuggested_folders_t *suggestedFldrs = psync_get_sync_suggestions();
    for (int i = 0; i < suggestedFldrs->entrycnt; i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        item->setCheckState(0,Qt::Unchecked);
        item->setData(0,Qt::UserRole,false);
        item->setText(1,suggestedFldrs->entries[i].localpath);
        item->setData(1,Qt::UserRole,suggestedFldrs->entries[i].localpath);
        QString rootName = checkRemoteName(suggestedFldrs->entries[i].name);
        remoteFldrsNamesLst.append(rootName);
        newRemoteFldrsLst.append(rootName);
        rootName.insert(0,"/");
        item->setText(3,rootName);
        item->setData(3,Qt::UserRole,rootName);
        //deled
        item->setData(2,Qt::UserRole,PSYNC_FULL -1 );
    }
    */
}

//slots
void WelcomeWin::changeCurrItem(QModelIndex index)
{
    if(index.row()) // the first item in the treeview is default and shoudn't be modified
    {
        this->isChangingItem = true;
        emit this->addSync();
    }
}
void WelcomeWin::closeEvent(QCloseEvent *event)
{
    this->finish();
    event->ignore();
}

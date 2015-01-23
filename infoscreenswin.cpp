#include "infoscreenswin.h"
#include "ui_infoscreenswin.h"
#include "common.h"
#include "pcloudapp.h"


InfoScreensWin::InfoScreensWin(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InfoScreensWin)
{
    ui->setupUi(this);
    app = a;
    ui->checkBox_showagain->setChecked(Qt::Checked);
    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Drive");
    this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

#ifdef Q_OS_LINUX
    ui->label_intro->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeIntro.png"));
    ui->label_drive->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeDrive.png"));
    ui->label_sync->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeSync.png"));
    ui->label_shares->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeShares.png"));
    ui->label_othrs->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeOthers.png"));
    ui->label_finish->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeFinish.png"));
#else
    ui->label_intro->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeIntroXP.png"));
    ui->label_drive->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeDriveXP.png"));
    ui->label_sync->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeSyncXP.png"));
    ui->label_shares->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeSharesXP.png"));
    ui->label_othrs->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeOthersXP.png"));
    ui->label_finish->setPixmap(QPixmap(":/welcomes/images/welcomes/welcomeFinishXP.png"));
#endif

    connect(ui->pagesWidget, SIGNAL(currentChanged(int)), this, SLOT(changePageContent(int)));
    connect(ui->btnPreviuos, SIGNAL(clicked()), this, SLOT(openPreviousPage()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(openNextPage()));
    connect(ui->btnOpenFldr, SIGNAL(clicked()), this, SLOT(finish()));
}

InfoScreensWin::~InfoScreensWin()
{
    delete ui;
}

void InfoScreensWin::showEvent(QShowEvent *event)
{
    ui->pagesWidget->setCurrentIndex(0);
    ui->btnOpenFldr->setVisible(false);
    ui->btnPreviuos->setVisible(false);
    ui->btnNext->setVisible(true);
    if(app->isFirstLaunch)
    {
        emit this->createDfltSync();
        ui->checkBox_showagain->setChecked(Qt::Checked);
    }
    event->accept();
}

//slots
void InfoScreensWin::changePageContent(int index)
{
    switch(index)
    {
    case WELCOME_INTRO_PAGE_NUM:
        qDebug()<<WELCOME_INTRO_PAGE_NUM<<"WELCOME_INTRO_PAGE_NUM";
        ui->btnPreviuos->setVisible(false);
        break;
    case DRIVE_INFO_PAGE_NUM:
        qDebug()<<DRIVE_INFO_PAGE_NUM<<"DRIVE_INFO_PAGE_NUM";
        ui->btnPreviuos->setVisible(true);
        break;
    case OTHERS_PLATFORMS_PAGE_NUM:
        qDebug()<<OTHERS_PLATFORMS_PAGE_NUM<<"OTHERS_PLATFORMS_PAGE_NUM";
        ui->btnOpenFldr->setVisible(false); //if was on last page
        ui->btnNext->setVisible(true);
        break;
    case FINISH_INFO_PAGE_NUM:
        qDebug()<<FINISH_INFO_PAGE_NUM<<"FINISH_INFO_PAGE_NUM";
        ui->btnNext->setVisible(false);
        ui->btnOpenFldr->setVisible(true);
        break;
    default:
        return;
    }
}

void InfoScreensWin::openPreviousPage()
{
    int currentIndex = ui->pagesWidget->currentIndex();
    ui->pagesWidget->setCurrentIndex(currentIndex - 1);
}

void InfoScreensWin::openNextPage()
{
    int currentIndex = ui->pagesWidget->currentIndex();
    ui->pagesWidget->setCurrentIndex(currentIndex + 1);
}

void InfoScreensWin::finish()
{    
    if(ui->checkBox_showagain->isChecked())
        this->app->settings->setValue("showintrowin",true);
    else if(app->settings->contains("showintrowin"))
        app->settings->remove("showintrowin");
    app->openCloudDir();
    this->close();
    if(app->settings->contains("autostartcrypto") && app->settings->value("autostartcrypto").toBool())
        app->requestCryptoKey();
}

void InfoScreensWin::createDfltSync()
{
    QString dfltRemotePath = "/pCloud Sync";
    pentry_t *pfldr = psync_stat_path(dfltRemotePath.toUtf8());
    if(pfldr == NULL)
    {
        char *err = NULL;
        psync_create_remote_folder_by_path(dfltRemotePath.toUtf8(),&err);
        if (err)
        {
            qDebug()<<"create psync default fldr err:"<<err;
            return;
        }
        free(err);
    }

    QDir *dfltLocalDir;
#ifdef Q_OS_WIN
    dfltLocalDir = new QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
#else
    dfltLocalDir = new QDir(QDir::homePath());
#endif
    QString localpath = dfltLocalDir->path().append(OSPathSeparator).append("pCloud Sync");
    QDir pcloudDir(localpath);
    localpath = dfltLocalDir->toNativeSeparators(localpath);

    if(!pcloudDir.exists())
    {
        dfltLocalDir->mkdir("pCloud Sync");
    }

    psync_syncid_t id = psync_add_sync_by_path(localpath.toUtf8(),dfltRemotePath.toUtf8(), PSYNC_FULL);
    if (id == -1)
        app->check_error();
    else
        app->pCloudWin->get_sync_page()->load();
}

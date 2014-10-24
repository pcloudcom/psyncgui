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
    ui->pagesWidget->setCurrentIndex(0);
    ui->btnPreviuos->setVisible(false);
    ui->checkBox_showagain->setChecked(Qt::Checked);

    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud Sync");
    this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    connect(ui->pagesWidget, SIGNAL(currentChanged(int)), this, SLOT(changePageContent(int)));
    ui->btnOpenFldr->setVisible(false);

    connect(ui->btnPreviuos, SIGNAL(clicked()), this, SLOT(openPreviousPage()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(openNextPage()));
   // connect(ui->btnFinish, SIGNAL(clicked()), this, SLOT(finish()));
    //connect(ui->btnOpenFldr, SIGNAL(clicked()), app, SLOT(openCloudDir()));
    connect(ui->btnOpenFldr, SIGNAL(clicked()), this, SLOT(finish()));

    //this->setAttribute(Qt::WA_DeleteOnClose, true);
}

InfoScreensWin::~InfoScreensWin()
{
    delete ui;
}


//slots
void InfoScreensWin::changePageContent(int index)
{
    switch(index)
    {
    case DRIVE_INFO_PAGE_NUM:
        qDebug()<<DRIVE_INFO_PAGE_NUM<<"DRIVE_INFO_PAGE_NUM";
        ui->btnPreviuos->setVisible(false);
        break;
    case SYNC_INFO_PAGE_NUM:
        qDebug()<<SYNC_INFO_PAGE_NUM<<"SYNC_INFO_PAGE_NUM";
        ui->btnPreviuos->setVisible(true);
        break;
    case SHARES_INFO_PAGE_NUM:
        qDebug()<<SHARES_INFO_PAGE_NUM<<"SHARES_INFO_PAGE_NUM";
        return;
    case OTHERS_PLATFORMS_PAGE_NUM:
        qDebug()<<OTHERS_PLATFORMS_PAGE_NUM<<"OTHERS_PLATFORMS_PAGE_NUM";
        ui->btnOpenFldr->setVisible(false); //if was on last page
       // ui->btnPreviuos->setVisible(true);
        ui->btnNext->setVisible(true);
        break;
    case FINISH_INFO_PAGE_NUM:
        qDebug()<<FINISH_INFO_PAGE_NUM<<"FINISH_INFO_PAGE_NUM";
        ui->btnNext->setVisible(false);
        ui->btnOpenFldr->setVisible(true);
        break;
    default:
        qDebug()<<"InfoScreensWin::changePageContent default";
        break;
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
    //++ save checkbox
    if(ui->checkBox_showagain->isChecked())
        this->app->settings->setValue("showintrowin",true);
    else if(app->settings->contains("showintrowin"))
        app->settings->remove("showintrowin");
    app->openCloudDir();
    this->close();
}

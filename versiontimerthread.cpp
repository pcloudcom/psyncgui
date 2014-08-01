#include "versiontimerthread.h"
#include "pcloudapp.h"

VersionTimerThread::VersionTimerThread(PCloudApp *a,QDateTime d) // d is the current time at the moment of creating the thread
{    
    app = a;
    connect(this, SIGNAL(sendTrayMsg(QString,QString)), app, SLOT(showTrayMessage(QString,QString)));
    connect(this, SIGNAL(restartTimer(int)), app, SLOT(setTimerInterval(int)));
    QDateTime now  = QDateTime::currentDateTime();

    // check if for ssettings set date notifications is missed because of sleep, app close or ect.
    if(d >= now)
        dateTimeForNotify = d;
    else // d is before now
    {
        dateTimeForNotify = QDateTime::currentDateTime();
        app->settings->setValue("vrsnNotfyDtTime",dateTimeForNotify);
        qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS(Timer): time for notifications was missed because of pc sleep, closed app or etc and new notify time is set to Current Time";
    }
}

void VersionTimerThread::run()
{
    qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (Timer started): time for notify: "<< dateTimeForNotify;
    while(true)
    {
        sleep(1); // 60 to emit the signal
        if(dateTimeForNotify.isNull())
        {
            qDebug()<< "NOTIFICATIONS (Timer): time thread stopped because time is null";
            this->quit();
        }
        QDateTime now = QDateTime::currentDateTime();

        if(now > dateTimeForNotify)
        {
            qDebug()<<QDateTime::currentDateTime() <<"NOTIFICATIONS (Timer): Tray Notification shown";
            emit sendTrayMsg("New Version", "A new version of pCloud Sync is available!\nClick here for more details");
            //start new reminder timer for the next interval
            emit restartTimer(app->settings->value("vrsnNotifyInvervalIndx").toInt());
        }
    }
}

void VersionTimerThread::setNewDateTimeForNotify(QDateTime d)
{
    this->dateTimeForNotify = d;
}

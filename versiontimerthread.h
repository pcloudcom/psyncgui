#ifndef VERSIONTIMERTHREAD_H
#define VERSIONTIMERTHREAD_H

#include<QThread>
#include<QDateTime>

class PCloudApp;

class VersionTimerThread : public QThread
{
    Q_OBJECT
public:
    explicit VersionTimerThread(PCloudApp *a, QDateTime d);
    void setNewDateTimeForNotify(QDateTime d);
private:
    PCloudApp *app;
    QDateTime dateTimeForNotify;
protected:
    void run();
signals:
    void sendTrayMsg(QString title, QString msg);
    void restartTimer(int index);
};

#endif // VERSIONTIMERTHREAD_H

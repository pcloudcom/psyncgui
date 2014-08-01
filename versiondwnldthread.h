#ifndef VERSIONDWNLDTHREAD_H
#define VERSIONDWNLDTHREAD_H

#include <QThread>

class VersionDwnldThread : public QThread
{
    Q_OBJECT
public:
    explicit VersionDwnldThread(const char* os,QObject *parent = 0);
protected:
    void run();
private:
    const char* OSStr;
signals:
    
public slots:
    
};

#endif // VERSIONDWNLDTHREAD_H

#include "versiondwnldthread.h"
#include "psynclib.h"
#include <QDebug>

VersionDwnldThread::VersionDwnldThread(const char* os,QObject *parent) :
    QThread(parent)
{
    OSStr = os;
}

void VersionDwnldThread::run()
{
    psync_new_version_t* newversion = psync_check_new_version_download_str(OSStr, APP_VERSION);
    if (newversion == NULL)
    {
        qDebug()<<"Qt VersionDwnldThread run: error donwloading new version";
        return;
    }
    psync_run_new_version(newversion);
    free(newversion);
}

#ifndef SHELLEXTTHREAD_H
#define SHELLEXTTHREAD_H

#include<QThread>
#ifdef Q_OS_WIN
#include <windows.h>
#include <aclapi.h>
#endif
class PCloudApp;

class ShellExtThread: public QThread
{
    Q_OBJECT
public:
    ShellExtThread(PCloudApp *a);
    ~ShellExtThread();
private:
    bool closePipe();
    void killPipe();
    PCloudApp *app;
    HANDLE hPipe;
protected:
    void run();
};

#endif // SHELLEXTTHREAD_H

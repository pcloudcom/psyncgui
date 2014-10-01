#ifndef SHELLEXTTHREAD_H
#define SHELLEXTTHREAD_H

#include<QThread>
#ifdef Q_OS_WIN
#include <windows.h>
#include <aclapi.h>

class PCloudApp;

class ShellExtThread: public QThread
{
    Q_OBJECT
public:
    ShellExtThread(PCloudApp *a);
    ~ShellExtThread();
    void killPipe();
private:
    bool closePipe();
    PCloudApp *app;
    HANDLE hPipe;
    bool localFldrsFlag; //flag identifying are local fodlers are seected or in pdrive
protected:
    void run();
};
#endif
#endif // SHELLEXTTHREAD_H

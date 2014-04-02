#include "pcloudapp.h"
//#include "mylogger.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
bool isRunning(){
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    DWORD id = GetCurrentProcessId();
    WCHAR processName[MAX_PATH] = {0};

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry)){
        while (Process32Next(snapshot, &entry)){
            if (entry.th32ProcessID == id){
                wcscpy(processName, entry.szExeFile);
                break;
            }
        }
    }

    if (processName[0] && Process32First(snapshot, &entry)){
        while (Process32Next(snapshot, &entry)){
            if (entry.th32ProcessID != id && !wcsicmp(processName, entry.szExeFile)){
                CloseHandle(snapshot);
                return true;
            }
        }
    }

    CloseHandle(snapshot);
    return false;
}
#endif

int main(int argc, char *argv[])
{    
#ifdef Q_OS_WIN
    if (isRunning()){
        MessageBoxA(NULL, "PCloud is already running.", "Already running", MB_OK);
        return 1;
    }
#endif
  //  MyLogger logger;
    return PCloudApp(argc, argv).exec();
}

#include "pcloudapp.h"
#include "mylogger.h"
#include "psynclib.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>

BOOL isMainWindowd(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lParam)
{
    if (hwnd == NULL)
    {
        return FALSE;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd,&processId);

    HWND parent = NULL;
    //pCloud Sync Main
    if(processId == (DWORD)lParam)
    {

        char wn[255];
        int iLen = GetWindowTextA(hwnd, wn, 255);
        if ( iLen > 0)
            qDebug()<<"first"<<wn;
        SendMessageW(hwnd,WM_USER+1,0,666);

        //SendMessage(hwnd,WM_SHOWWINDOW,0,0);


        while(parent = GetParent(hwnd))
            hwnd = parent;
        qDebug()<<"Qt: second launch" << GetLastError();
        //ShowWindow(hwnd,SW_SHOW);
        if (isMainWindowd(hwnd))
        {
            char wn[255];
            int iLen = GetWindowTextA(hwnd, wn, 255);
            if ( iLen > 0)
                qDebug()<<"second"<<wn;
            //SendMessage(hwnd,WM_SYSCOMMAND,0,0);
            //SendMessageA
            // SendMessageW(hwnd,WM_NOTIFY,0,0);
            //PostMessageW(hwnd,WM_COMMAND,0,0);
        }
    }

    return TRUE;
}

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
                EnumWindows(EnumProc,entry.th32ProcessID);
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
    if (psync_init() == -1)
    {
        qDebug()<<"Init sync returned failed. It may be already running! "<<psync_get_last_error();
#ifdef Q_OS_WIN
        MyLogger logger;
        if (isRunning())
            MessageBoxA(NULL, "pCloud Drive is already running.", "Already running", MB_OK);
#endif
        return 1;
    }

    return PCloudApp(argc, argv).exec();
}

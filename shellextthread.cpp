#include "shellextthread.h"
#include "pcloudapp.h"

#ifdef Q_OS_WIN
#include <Shlobj.h>


#include "QMessageBox"


#define  PIPE_NAME L"\\\\.\\pipe\\shellextnpipe1"
#define BUFSIZE 512

ShellExtThread::ShellExtThread(PCloudApp *a)
{
    app = a;

    PSID pEveryoneSID = NULL;
    PACL pACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea[1];
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = {SECURITY_WORLD_SID_AUTHORITY};
    SECURITY_ATTRIBUTES sa;

    if(!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
    {
        goto cleanup;
    }

    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance= NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

    if (SetEntriesInAcl(1, ea, NULL, &pACL))
    {
        goto cleanup;
    }

    pSD = (PSECURITY_DESCRIPTOR) malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
    if(!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
    {
        goto cleanup;
    }
    if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
    {
        goto cleanup;
    }
    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle = FALSE;

    qDebug() << "Qt: creating pipe";
    hPipe = CreateNamedPipe(PIPE_NAME,  PIPE_ACCESS_DUPLEX,
                            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                            1, BUFSIZE,BUFSIZE, 0, &sa);
    qDebug()<< "Qt: pipe created successfully";
    if (hPipe != INVALID_HANDLE_VALUE)
        return;

cleanup:
    if (pEveryoneSID) FreeSid(pEveryoneSID);
    if (pACL) free(pACL);
    if (pSD) free(pSD);
}

void ShellExtThread::run()
{    
    qDebug()<< " Qt: RUN thread pipe started";
    while(true)
    {
        qDebug()<< "Qt WHILE: pipe started  ";
        if (hPipe != INVALID_HANDLE_VALUE)
        {
            qDebug()<< "Qt: pipe is VALID handle, before connect";
            if (!ConnectNamedPipe(hPipe, NULL) && (GetLastError() != ERROR_PIPE_CONNECTED))
            {
                qDebug()<< "Qt: Connect pipe err: "<< GetLastError();
                break;
            }
            else
                qDebug() << "Qt: pipe sucessfully connected";
        }
        else
        {
            qDebug()<<"qt CreateNamedPipe failed pipe is invalid handle, GLE=%d.\n" << GetLastError();
            return;
        }

        qDebug() << "Qt: before read";
        char buffer[BUFSIZE];
        DWORD numBytesRead = 0;

        bool resread = ReadFile(hPipe, buffer, BUFSIZE, &numBytesRead, NULL);
        qDebug() << "Qt: after read";
        if (resread)
        {
            QString bufferStr = buffer;
            qDebug()<<"Qt: read file not zero";

            //SyncLIST
            if (bufferStr.startsWith("synclist"))
            {
                qDebug() << "Qt: read pipe: synclist case";
                //send synclist to the client
                psync_folder_list_t *fldrsList = psync_get_sync_list();
                qDebug() << "Qt: read pipe: synclist case after getsynclist from the lib";
                if (fldrsList != NULL && fldrsList->foldercnt)
                {
                    char msgCopyFldrs[512* fldrsList->foldercnt],msgDwnldFldrs[512*fldrsList->foldercnt];
                    strcpy(msgCopyFldrs, "synclist");
                    strcpy(msgDwnldFldrs,"");
                    qDebug()<< "Qt: synlict write before fill list"<< strlen(msgCopyFldrs);
                    for (uint i = 0; i< fldrsList->foldercnt; i++)
                    {
                        if(fldrsList->folders[i].synctype != PSYNC_DOWNLOAD_ONLY)
                        {
                            strcat(msgCopyFldrs,fldrsList->folders[i].localpath);
                            strcat(msgCopyFldrs,"|");
                            qDebug()<<"Qt: write in pipe - synced folder"<< fldrsList->folders[i].localpath<< "sync type: "<<fldrsList->folders[i].synctype;
                        }
                        else
                        {
                            strcat(msgDwnldFldrs,fldrsList->folders[i].localpath);
                            strcat(msgDwnldFldrs,"*");
                            qDebug()<<"Qt: write in pipe - synced donwload only folder"<< fldrsList->folders[i].localpath<< "sync type: "<<fldrsList->folders[i].synctype;
                        }
                    }

                    strcat(msgCopyFldrs, msgDwnldFldrs);
                    qDebug() << "Qt: whole sync list:" << msgCopyFldrs;
                    DWORD numBytesWritten = 0;
                    wchar_t wtext[strlen(msgCopyFldrs)];
                    mbstowcs(wtext, msgCopyFldrs,strlen(msgCopyFldrs)+1);//Plus null

                    //send both lists, after every copy folders there is  '|', after every donwload  only folder  there is '*'
                    bool result = WriteFile(
                                hPipe, // handle to our outbound pipe
                                msgCopyFldrs, // data to send
                                strlen(msgCopyFldrs)*sizeof(char),// length of data to send (bytes)
                                &numBytesWritten, // will store actual amount of data sent
                                NULL); // not using overlapped IO

                    if (result)
                        qDebug()<<"Qt: write copy synced list successful";
                    else
                        qDebug()<<"Qt: write copy synced list UNsuccessful";
                }
            }
            //ADD NEW SYNC
            else if(bufferStr.startsWith("addsync"))
            {
                qDebug() << "Qt:read pipe addnew sync case";
                bufferStr.remove(0,7); // removes "addsync"
                qDebug()<<bufferStr << "the path only";
                app->localpathToSync = bufferStr;
                if (app->isLogedIn())
                    app->addNewSyncPublic();
                else
                    app->showLoginPublic();
            }
            //REMOVE FROM SYNC
            else if(bufferStr.startsWith("remove "))
            {
                bufferStr.remove(0,7);
                qDebug()<< "Qt: remove from sync folder:"<< bufferStr;
                if(app->isLogedIn())
                {
                    psync_folder_list_t *fldrsList = psync_get_sync_list();
                    psync_syncid_t id;
                    for (size_t i = 0; i < fldrsList->foldercnt; i++)
                    {
                        if(bufferStr == fldrsList->folders[i].localpath)
                        {
                            id = fldrsList->folders[i].syncid;
                            break;
                        }
                    }
                    psync_delete_sync(id);
                    qDebug() << "Qt: Delete Sync with id = " << id;
                }
                else
                    app->showLoginPublic();
            }
        }
        else
        {
            qDebug()<< "qt read failed gle=" <<GetLastError();
            if (numBytesRead == 0 && (GetLastError() == ERROR_BROKEN_PIPE))
            {
                qDebug()<<"qt InstanceThread: server disconnected.\n " << GetLastError();
            }
            else
            {
                qDebug() << "qt InstanceThread ReadFile failed, GLE=%d.\n "<<GetLastError();
            }
        }
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);

    }

    if (hPipe != INVALID_HANDLE_VALUE){
        qDebug()<< "qt after while: flush and disconnect handle";
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
    }

    /*  if (hPipe != INVALID_HANDLE_VALUE){
        qDebug()<< "qt after while: flush and disconnect handle";
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }
    */


}

ShellExtThread::~ShellExtThread()
{
    qDebug()<< "qt destructing pipe";
    if (hPipe != INVALID_HANDLE_VALUE){
        closePipe();
        CloseHandle(hPipe);
        qDebug() << " pipe closed";
        hPipe = INVALID_HANDLE_VALUE;
    }
    qDebug()<<" pipe destroyed";
}

bool ShellExtThread::closePipe()
{
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        if (FlushFileBuffers(hPipe) && DisconnectNamedPipe(hPipe))
            return true;
    }
    return false;
}

void ShellExtThread::killPipe()
{
    if(closePipe() && hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

#endif

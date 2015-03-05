#include "shellextthread.h"
#include "pcloudapp.h"

#ifdef Q_OS_WIN
#include <Shlobj.h>
#include "QMessageBox"


#define  PIPE_NAME L"\\\\.\\pipe\\shellextnpipe2"
#define BUFSIZE 8192 + MAX_PATH + 10 //crypto

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

        char buffer[BUFSIZE];
        DWORD numBytesRead = 0;

        bool resread = ReadFile(hPipe, buffer, BUFSIZE, &numBytesRead, NULL); // in cycle while read for long msgs
        qDebug() << "Qt: after read";
        if (resread)
        {
            QString bufferStr = buffer;
            qDebug()<<"Qt: read file not zero, received buffer ="<<bufferStr;

            //NOT LOGGED
            if (bufferStr.startsWith("login"))
                app->showLoginPublic();

            //SyncLIST
            else if (bufferStr.startsWith("synclist"))
            {
                qDebug() << "Qt: read pipe: request the synclist case" << bufferStr;
                //send synclist to the client

                bool isCrypto = false, canShare = true, multiselect = false;
                char msgCopyFldrs[BUFSIZE];
                if (app->isLogedIn())
                {
                    QString remoteFldr("");
                    if(bufferStr[8] != '\0') // clicked in drive
                    {
                        localFldrsFlag = false;
                        if(bufferStr[8] == '*') //check crypto
                        {
                            if(bufferStr[9] == '*')
                            {
                                multiselect = true;
                                remoteFldr = bufferStr.replace("\\","/").remove(0,12);
                            }
                            else
                                remoteFldr = bufferStr.replace("\\","/").remove(0,11);
                            if(remoteFldr.startsWith("pCloud/pCloud Drive",Qt::CaseInsensitive)) //remove network preffix
                            {
                                remoteFldr.remove(0,19);
                                bufferStr.remove(0,19);
                            }

                            int i = -1;
                            //check is selected in crypto subfodler
                            while(remoteFldr.contains("/"))
                            {
                                remoteFldr = remoteFldr.section("/", 0, i);
                                pentry_t* pfldr =  psync_stat_path(remoteFldr.toUtf8());
                                if (pfldr != NULL && pfldr->folder.isencrypted)
                                {
                                    isCrypto = true;
                                    break;
                                    qDebug()<<"Qt is crypto" <<isCrypto<<multiselect;
                                }
                                i--;
                            }
                        }
                    }
                    else
                        localFldrsFlag = true;

                    strcpy(msgCopyFldrs, "synclist");
                    if (isCrypto && (bufferStr == remoteFldr) && !multiselect)  //crypto folder is selected
                    {
                        canShare = false;
                        strcat(msgCopyFldrs,"0"); //canshare
                        strcat(msgCopyFldrs,":");

                        if(psync_crypto_isstarted())
                            strcat(msgCopyFldrs,"0"); //folder is unlocked
                        else
                            strcat(msgCopyFldrs,"1");

                        strcat(msgCopyFldrs,"?"); //separator for crypto locked status
                    }
                    else if (isCrypto && (bufferStr != remoteFldr)) //selection in crypto folder
                    {
                        canShare = false;
                        strcat(msgCopyFldrs,">");
                    }
                    else // no crypto selection
                    {
                        psync_folder_list_t *fldrsList = psync_get_sync_list();
                        if (fldrsList != NULL && fldrsList->foldercnt)
                        {
                            if(localFldrsFlag)
                            {
                                char msgDwnldOnlyFldrs[260*fldrsList->foldercnt];
                                strcpy(msgDwnldOnlyFldrs,"");

                                for (uint i = 0; i< fldrsList->foldercnt; i++)
                                {
                                    if(fldrsList->folders[i].synctype != PSYNC_DOWNLOAD_ONLY)
                                    {
                                        strcat(msgCopyFldrs,fldrsList->folders[i].localpath);
                                        strcat(msgCopyFldrs,"|");
                                    }
                                    else
                                    {
                                        strcat(msgDwnldOnlyFldrs,fldrsList->folders[i].localpath);
                                        strcat(msgDwnldOnlyFldrs,"*");
                                    }
                                }
                                strcat(msgCopyFldrs, msgDwnldOnlyFldrs); //concatenate folders lists
                                qDebug() << "Qt: whole sync list for send locals :" << msgCopyFldrs;
                            }
                            else
                            {
                                for (uint i = 0; i< fldrsList->foldercnt; i++)
                                {
                                    QString remotePathStr = fldrsList->folders[i].remotepath;
                                    qDebug()<<"Qt: read pipe: add remote folder to send" <<remotePathStr.toUtf8().constData();
                                    strcat(msgCopyFldrs, remotePathStr.toUtf8().constData());
                                    strcat(msgCopyFldrs,"|");
                                }
                            }
                        }
                        free(fldrsList);
                    }
                    qDebug() << "Qt: whole sync list for send remote :" << msgCopyFldrs;
                }
                else
                    strcpy(msgCopyFldrs,"notlogged");

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
                    qDebug()<<"Qt: write in pipe(send synclist) successful, synclist = "<<msgCopyFldrs;
                else
                    qDebug()<<"Qt: write in pipe(send synclist) NOT successful";
            }
            //ADD NEW SYNC
            else if(bufferStr.startsWith("addsync")) // local fldrs
            {
                qDebug() << "Qt:read pipe addnew sync case" <<bufferStr <<localFldrsFlag;
                bufferStr.remove(0,7); // removes "addsync"
                if (app->isLogedIn())
                {
                    QStringList fldrToSyncLst = bufferStr.split("|", QString::SkipEmptyParts);
                    app->setsyncSuggstLst(fldrToSyncLst);
                    app->addNewSyncLstPublic(localFldrsFlag);
                }
                else
                    app->showLoginPublic();
            }
            //REMOVE FROM SYNC
            else if(bufferStr.startsWith("remove ")) //local fldrs
            {
                bufferStr.remove(0,7);
                qDebug()<< "Qt: remove from sync folders:"<< bufferStr;
                if(app->isLogedIn())
                {
                    QStringList fldrsToRemoveLst = bufferStr.split("|",QString::SkipEmptyParts);
                    psync_folder_list_t *fldrsList = psync_get_sync_list();
                    if (localFldrsFlag)
                    {
                        for (int i = 0; i < fldrsToRemoveLst.size(); i++)
                        {
                            QString fldr = fldrsToRemoveLst.at(i);
                            for(int j = 0; j < fldrsList->foldercnt; j++) //search syncid of the synced folder
                            {
                                if(fldr == fldrsList->folders[j].localpath)
                                {
                                    psync_syncid_t id = fldrsList->folders[j].syncid;
                                    psync_delete_sync(id);
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < fldrsToRemoveLst.size(); i++)
                        {
                            QString fldr = fldrsToRemoveLst.at(i);
                            for(int j = 0; j < fldrsList->foldercnt; j++) //search syncid of the synced folder
                            {
                                if(fldr == fldrsList->folders[j].remotepath)
                                {
                                    psync_syncid_t id = fldrsList->folders[j].syncid;
                                    psync_delete_sync(id);
                                    break;
                                }
                            }
                        }
                    }
                    free(fldrsList);
                    app->refreshSyncUIitemsPublic(); //refresh synced folders list in syncmenu
                }
                else
                    app->showLoginPublic();
            }
            //SHARE FOLDER
            else if(bufferStr.startsWith("sharefr"))
            {
                qDebug()<<"Qt: share folder case"<< bufferStr;
                app->addNewSharePublic(bufferStr.remove(0,9).replace("\\","/")); // removes p: also
            }
            //MANAGE CRYPTO FLDR
            else if(bufferStr.startsWith("managecryptofldr"))
            {
                if(psync_crypto_isstarted())
                    app->lockCryptoFldrPublic();
                else
                    app->unlockCryptoFldrPublic();
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
}

ShellExtThread::~ShellExtThread()
{
    qDebug()<< "qt destructing pipe";
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        closePipe();
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
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
        qDebug()<<"killpipe: handle closed";
        hPipe = INVALID_HANDLE_VALUE;
    }
}

#endif

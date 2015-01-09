#ifndef COMMON_H
#define COMMON_H

//const char* typeStr[3]={"Donwload only", "Upload only", "Download and Upload"};

#define WINDOW_ICON ":/images/images/logo.png"
//#define WINDOW_ICON ":/16x16/images/16x16/logo.png"
#define ONLINE_ICON ":/24x24/images/24x24/online.png"
#define OFFLINE_ICON ":/24x24/images/24x24/offline.png"
#define SYNCED_ICON ":/24x24/images/24x24/synced.png"
#define SYNCING_ICON ":/24x24/images/24x24/syncing.png"
#define PAUSED_ICON ":/24x24/images/24x24/sync-paused.png"
#define SYNC_FULL_ICON ":/24x24/images/24x24/sync-full.png"

#define HELP_ICON ":/128x128/images/128x128/help.png"
#define INFO_ICON ":/128x128/images/128x128/info.png"
#define USER_ICON ":/128x128/images/128x128/user.png"
#define SYNC_TAB_ICON ":/128x128/images/128x128/sync.png"


//eachs page num according to index in pcloudwin
#define ACCNT_LOGGED_PAGE_NUM 0
#define SYNC_PAGE_NUM 1
#define SHARES_PAGE_NUM 2
#define CRYPTO_PAGE_NUM 3
#define SETTINGS_PAGE_NUM 4
#define HELP_PAGE_NUM 5
#define ABOUT_PAGE_NUM 6

//first screens pages
#define WELCOME_INTRO_PAGE_NUM 0
#define DRIVE_INFO_PAGE_NUM 1
#define SYNC_INFO_PAGE_NUM 2
#define SHARES_INFO_PAGE_NUM 3
#define OTHERS_PLATFORMS_PAGE_NUM 4
#define FINISH_INFO_PAGE_NUM 5


#ifdef Q_OS_WIN
#define OSPathSeparator "\\"
#else
#define OSPathSeparator "/"
#endif


#endif


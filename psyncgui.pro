#-------------------------------------------------
#
# Project created by QtCreator 2014-02-24T15:45:58
#
#-------------------------------------------------

QT       += core gui
QT +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = psyncgui
TEMPLATE = app
target.path = /usr/bin
INSTALLS += target


VERSION = 1.1.3
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += DEBUG_LEVEL=D_NOTICE
DEFINES += P_OS_WINDOWS
DEFINES += _FILE_OFFSET_BITS=64

SOURCES += main.cpp\
        loginwindow.cpp \
    registerwindow.cpp \
    pcloudapp.cpp \
    addsyncdialog.cpp \
    changepassdialog.cpp \
    modifysyncdialog.cpp \
    pcloudwindow.cpp \
    psettings.cpp \
    syncpage.cpp \
    mylogger.cpp \
    shellextthread.cpp\
    settingspage.cpp \
    versiontimerthread.cpp \
    suggestnsbasewin.cpp \
    welcomewin.cpp \
    versiondwnldthread.cpp

HEADERS  += loginwindow.h \
    registerwindow.h \
    pcloudapp.h \
    addsyncdialog.h \
    changepassdialog.h \
    modifysyncdialog.h \
    pcloudwindow.h \
    psettings.h \
    syncpage.h \
    common.h\
    mylogger.h \
    shellextthread.h \
    settingspage.h \
    versiontimerthread.h \
    suggestnsbasewin.h \
    welcomewin.h \
    versiondwnldthread.h

FORMS    += loginwindow.ui \
    registerwindow.ui \
    addsyncdialog.ui \
    changepassdialog.ui \
    modifysyncdialog.ui \
    pcloudwindow.ui \
    suggestnsbasewin.ui
win32:RC_FILE= pCloud.rc
RESOURCES += \
    Resources.qrc
win32{
LIBS += -L$$PWD/.. -lpsync
LIBS += -lssl -lcrypto -lsqlite -liphlpapi

INCLUDEPATH += $$PWD/.. $$PWD/../pclsync
}

unix{
LIBS += -lssl -lcrypto -lsqlite3 -lfuse
LIBS += -L$$PWD/../../sync/pclsync -lpsync
#LIBS += $$PWD/../pclsync/psynclib.a

INCLUDEPATH += $$PWD/../../sync/pclsync
#INCLUDEPATH += $$PWD/../pclsync
}
QMAKE_CXXFLAGS += -g


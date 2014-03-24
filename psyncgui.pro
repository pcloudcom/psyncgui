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


VERSION = 1.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += DEBUG_LEVEL=D_NOTICE
DEFINES += P_OS_WINDOWS

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
    welcomescreen.cpp

HEADERS  += loginwindow.h \
    registerwindow.h \
    pcloudapp.h \
    addsyncdialog.h \
    changepassdialog.h \
    modifysyncdialog.h \
    pcloudwindow.h \
    psettings.h \
    syncpage.h \
    welcomescreen.h \
    common.h

FORMS    += loginwindow.ui \
    registerwindow.ui \
    addsyncdialog.ui \
    changepassdialog.ui \
    modifysyncdialog.ui \
    pcloudwindow.ui \
    welcomescreen.ui
win32:RC_FILE= pCloud.rc
RESOURCES += \
    Resources.qrc

LIBS += -lssl -lcrypto -lsqlite3
#LIBS += -L$$PWD/../../git/sync/pclsync -lpsync
LIBS += -L$$PWD/../../sync/pclsync -lpsync

#INCLUDEPATH += $$PWD/../../git/sync/pclsync
INCLUDEPATH += $$PWD/../../sync/pclsync

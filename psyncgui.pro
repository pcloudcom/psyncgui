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
    welcomescreen.cpp\
    mylogger.cpp \
    shellextthread.cpp\
    welcomescreen.cpp \
    settingspage.cpp

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
    common.h\
    mylogger.h \
    shellextthread.h
    common.h \
    settingspage.h

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

LIBS += -lssl -lcrypto -lsqlite3 -lfuse -liphlpapi
LIBS += -L$$PWD/.. -lpsync
#LIBS += -L$$PWD/../../git/sync/pclsync -lpsync
#LIBS += $$PWD/../pclsync/psynclib.a


#INCLUDEPATH += $$PWD/../../git/sync/pclsync
INCLUDEPATH += $$PWD/.. $$PWD/../pclsync
QMAKE_CXXFLAGS += -g


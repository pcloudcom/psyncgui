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
<<<<<<< HEAD
    welcomescreen.cpp\
    mylogger.cpp \
    shellextthread.cpp
=======
    welcomescreen.cpp \
    settingspage.cpp
>>>>>>> f274c3b1d391b06d15e3288f3d864d00454a6b5d

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
<<<<<<< HEAD
    common.h\
    mylogger.h \
    shellextthread.h
=======
    common.h \
    settingspage.h
>>>>>>> f274c3b1d391b06d15e3288f3d864d00454a6b5d

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

<<<<<<< HEAD
#LIBS += -lssl -lcrypto -lsqlite3
#LIBS += -L$$PWD/../../git/sync/pclsync -lpsync
LIBS += -L$$PWD/.. -lpsync
LIBS += -lssl -lcrypto -lsqlite -liphlpapi

#INCLUDEPATH += $$PWD/../../git/sync/pclsync
INCLUDEPATH += $$PWD/.. $$PWD/../pclsync
=======
LIBS += -lssl -lcrypto -lsqlite3 -lfuse
#LIBS += -L$$PWD/../../git/sync/pclsync -lpsync
LIBS += $$PWD/../pclsync/psynclib.a


#INCLUDEPATH += $$PWD/../../git/sync/pclsync
INCLUDEPATH += $$PWD/../pclsync
QMAKE_CXXFLAGS += -g
>>>>>>> f274c3b1d391b06d15e3288f3d864d00454a6b5d

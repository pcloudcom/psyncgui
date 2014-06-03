#include "mylogger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

#ifdef Q_OS_WIN
#include <QtMessageHandler>
//QFile file("c:/tmp/psyncguilog.txt");
QFile file(QDir::tempPath()+ "/psyncguilog.txt");
QTextStream out(&file);


static void SimpleLoggingHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
        file.open(QIODevice::Append | QIODevice::Text);
        out<<"Debug msg "<<QDateTime::currentDateTime().toString()<< " " <<msg<<"\n";
        out.flush();
        file.close();

        break;
    case QtCriticalMsg:
        file.open(QIODevice::Append | QIODevice::Text);
        out<<"Critical msg "<<QDateTime::currentDateTime().toString() << " "<<msg<<"\n";
        out.flush();
        file.close();
        break;
    case QtWarningMsg:
        file.open(QIODevice::Append | QIODevice::Text);
        out<<"Warning msg "<<QDateTime::currentDateTime().toString()<< " "<<msg<<"\n";
        out.flush();
        file.close();
        break;
    case QtFatalMsg:
        file.open(QIODevice::Append | QIODevice::Text);
        out<<"Fatal msg "<<QDateTime::currentDateTime().toString() <<" "<<msg<<"\n";
        out.flush();
        file.close();
        abort();
    }
}

MyLogger::MyLogger(QObject *parent) :
    QObject(parent)
{
    qInstallMessageHandler(SimpleLoggingHandler);
    qDebug()<<"from file";
}

#endif

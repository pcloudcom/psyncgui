#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <QObject>

#ifdef Q_OS_WIN

class MyLogger : public QObject
{
    Q_OBJECT
public:
    explicit MyLogger(QObject *parent = 0);
    
signals:
    
public slots:
    
};
#endif

#endif // MYLOGGER_H

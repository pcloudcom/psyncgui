#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <QObject>

class MyLogger : public QObject
{
    Q_OBJECT
public:
    explicit MyLogger(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // MYLOGGER_H

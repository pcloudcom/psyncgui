#ifndef WELCOMEWIN_H
#define WELCOMEWIN_H

#include <QMainWindow>
#include "suggestnsbasewin.h"
#include "ui_suggestnsbasewin.h"
#include <QStringList>

class PCloudApp;

class WelcomeWin : public SuggestnsBaseWin
{
    Q_OBJECT
public:
    explicit WelcomeWin(PCloudApp *a, QWidget *parent = 0);
    
private:
    //PCloudApp *app;
    //QStringList *fldrsLst;
protected:
     void closeEvent(QCloseEvent *event);
signals:
    
public slots:
      void changeCurrItem(QModelIndex index);
};

#endif // WELCOMEWIN_H

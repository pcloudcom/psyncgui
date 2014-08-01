#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H
#include "pcloudwindow.h"
#include "pcloudapp.h"

#include <QObject>
#include <QString>
#include <QSettings>
#include <QProcess>

class PCloudWindow;

class SettingsPage: public QObject
{
    Q_OBJECT
public:
  friend class PCloudWindow;
  explicit SettingsPage(PCloudWindow *w, PCloudApp *a, QObject *parent = 0);
private:
    PCloudWindow *win;
    PCloudApp *app;
    QString dir;
    QString initFlrd;
    QString initCache;
    quint32 cacheSize;
    bool contextMenu;
    void initSettingsPage();
    qint32 getCacheSize();
public slots:    
    void dirChange();       
    void saveSettings();
    void setSaveBtnEnable();
    void cancelSettings();
};

#endif // SETTINGSPAGE_H

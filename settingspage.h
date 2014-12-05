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
  void showEvent(); // custom
  void initPublic();
private:
    PCloudWindow *win;
    PCloudApp *app;
    bool p2p;
    int upldSpeed, upldSpeedNew, dwnldSpeed, dwnldSpeedNew;
    QString initFlrd;
    QString initCache;
    quint64 cacheSize, minLocalSpace;
    bool contextMenu;  
    void initSettingsPage();
    void initMain();
    void initSpeed();
    void initSpace();
    void clearSpeedEditLines();
    qint32 getCacheSize();        
public slots:        
    void saveSettings();
    void setSaveBtnEnable();
    void cancelSettings();
    void setNewDwnldSpeed();
    void setNewUpldSpeed();
};

#endif // SETTINGSPAGE_H

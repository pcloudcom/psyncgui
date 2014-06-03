#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H
#include "pcloudwindow.h"
#include "pcloudapp.h"

#include <QObject>
#include <QString>

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
    bool initSSL;
    void initSettingsPage();
public slots:
    void startFS(int n);
    void dirChange();
    void saveSettings();
    void setSaveBtnEnable();
    void cancelSettings();
};

#endif // SETTINGSPAGE_H

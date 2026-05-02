#ifndef MYNOTES_TRAYICONMANAGER_H
#define MYNOTES_TRAYICONMANAGER_H

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class Settings;

class TrayIconManager : public QObject {
    Q_OBJECT
public:
    explicit TrayIconManager(Settings* settings, QObject *parent = nullptr);
    void setup(QWidget* mainWindow);

signals:
    void showRequested();
    void settingsRequested();
    void aboutRequested();
    void quitRequested();
    void syncAllRequested();
#ifdef ENABLE_TROJAN
    void trojanToggled(bool checked);
#endif

private:
    void setAutoStart();
    QSystemTrayIcon* m_trayIcon;
    Settings* m_settings;
};

#endif

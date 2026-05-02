#include "appcore/TrayIconManager.h"
#include "settings/Settings.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QSettings>
#include <QCoreApplication>
#include <QUrl>

TrayIconManager::TrayIconManager(Settings* settings, QObject *parent)
    : QObject(parent)
    , m_trayIcon(new QSystemTrayIcon(this))
    , m_settings(settings)
{
}

void TrayIconManager::setup(QWidget* mainWindow)
{
    Q_UNUSED(mainWindow);
#ifdef Q_OS_MAC
    m_trayIcon->setIcon(QIcon(QPixmap(":/icon/tray_128x128.png")));
#else
    m_trayIcon->setIcon(QIcon(QPixmap(":/icon/notebook_128x128.png")));
#endif
    auto menu = new QMenu();
    menu->addAction(tr("Show"), [this](){
       emit showRequested();
    });

    menu->addAction(tr("Settings"), [this](){
        emit settingsRequested();
    });
#ifdef ENABLE_TROJAN
    auto a = new QAction("Trojan");
    a->setCheckable(true);
    menu->addAction(a);
    connect(a, &QAction::triggered, [this, a](){
        qDebug() << "trojan triggered" << a->isChecked();
        emit trojanToggled(a->isChecked());
    });
#endif
    menu->addAction(tr("Show Config"), [this]() {
        Settings::instance();
        auto path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).first();
        auto dir = QDir(path);
        dir.cdUp();
        QString url = QString("%1/MyNotes.ini").arg(dir.absolutePath());
        QDesktopServices::openUrl(QUrl(url));
    });
    menu->addAction(tr("About"), [this](){
        emit aboutRequested();
    });
    menu->addAction(tr("About Qt"), [this](){
        qApp->aboutQt();
    });
    menu->addAction(tr("Quit"), [this](){
        emit quitRequested();
    });
    m_trayIcon->setContextMenu(menu);
    m_trayIcon->show();
    connect(m_trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        qDebug() << reason;
        switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            emit showRequested();
            break;
        default:
            break;
        }
    });
    setAutoStart();
}

void TrayIconManager::setAutoStart() {
#ifdef Q_OS_WIN
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
    QString applicationName = QApplication::applicationName();
    auto settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    QString applicationPath = QApplication::applicationFilePath();
    settings->setValue(applicationName, '"' + applicationPath.replace("/", "\\") + '"');
    delete settings;
#endif
#ifdef Q_OS_MAC
    // mac开机自启动
    // https://gist.github.com/andreybutov/33783bca1af9db8f9f36c463c77d7a86
    auto macOSXAppBundlePath = []() {
        QDir dir = QDir (QCoreApplication::applicationDirPath() );
        dir.cdUp();
        dir.cdUp();
        QString absolutePath = dir.absolutePath();
        // absolutePath will contain a "/" at the end,
        // but we want the clean path to the .app bundle
        if ( absolutePath.length() > 0 && absolutePath.right(1) == "/" ) {
            absolutePath.chop(1);
        }
        return absolutePath;
    };
    QStringList args;
    args << QString("-e tell application \"System Events\" to make login item at end ") +
            "with properties {path:\"" + macOSXAppBundlePath() + "\", hidden:false}";

    QProcess::execute("osascript", args);
#endif
}

#include "Widget.h"
#include "SettingsDialog.h"
#include <QApplication>
int showWindow() {
    Widget w;
    w.show();
    return QApplication::exec();
}
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // QSettings会用到
    QApplication::setOrganizationName("PikachuHy");
    QApplication::setOrganizationDomain("pikachu.net.cn");
    QApplication::setApplicationName("MyNotes");
    bool needSetConfig = false;
    auto m_settings = Settings::instance();
    auto baseUrl = m_settings->value("server.base_url", QString()).toString();
    if (baseUrl.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "baseUrl:" << baseUrl;

    auto owner = m_settings->value("server.owner", QString()).toString();
    if (owner.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "owner:" << owner;
    auto port = m_settings->value("server.port", 0).toInt();
    if (port == 0) {
        needSetConfig = true;
    }
    qDebug() << "port:" << port;
    auto password = m_settings->value("server.password", 0).toInt();
    if (password == 0) {
        needSetConfig = true;
    }
    qDebug() << "password:" << password;
#ifdef Q_OS_WIN
    auto typoraPath = m_settings->value("path.typora", "").toString();
    if (typoraPath.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "typoraPath:" << typoraPath;
#endif
    if (needSetConfig) {
        auto settingsDialog = new SettingsDialog();
        auto ret = settingsDialog->exec();
        if (ret == QDialog::Accepted) {
            return showWindow();
        }
    } else {
        return showWindow();
    }
    return 0;
}

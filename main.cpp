#include "Widget.h"
#include "SettingsDialog.h"
#include <QApplication>
#include <Logger.h>
#include <ConsoleAppender.h>
#include <FileAppender.h>
#include <QStandardPaths>
#include <QDir>
#include "SingleApplication.h"
#include <QLoggingCategory>
#include <QCommandLineParser>
#include "config.h"
int showWindow(SingleApplication* app) {
    Widget w;
    QObject::connect(app, &SingleApplication::messageAvailable,
        [&w](QString message) {
        qDebug() << "show";
        w.showNormal();
    }
    );
    w.show();
    auto ret = QApplication::exec();
    LOG_WARNING() << "Something went wrong." << "Result code is" << ret;
    return ret;
}
int main(int argc, char *argv[]) {
    SingleApplication a(argc, argv, "MyNotes");
    if (a.isRunning()) {
        a.sendMessage("app in running");
        return 0;
    }
#ifdef _DEBUG
#else
    // 发布版默认不输出debug日志
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, false);
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    auto logPath = docPath + "/MyNotes/logs";
    QDir logDir(logPath);
    if (!logDir.exists()) {
        logDir.mkdir(logPath);
    }
    auto fileAppender = new FileAppender(logPath+"/log.txt");
    fileAppender->setFormat("[%{type:-7}] <%{Function}> %{message}\n");
    cuteLogger->registerAppender(fileAppender);
#endif
    // QSettings会用到
    QApplication::setOrganizationName("PikachuHy");
    QApplication::setOrganizationDomain("pikachu.net.cn");
    QApplication::setApplicationName("MyNotes");
    QApplication::setApplicationVersion(PROJECT_VERSION);
    // 解析工具
    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption debugOption(
            "debug",
            "output debug log");
    parser.addOption(debugOption);
    parser.process(a);
    const QStringList args = parser.positionalArguments();
    if (parser.isSet(debugOption)) {
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    }

    bool needSetConfig = false;
    auto m_settings = Settings::instance();
    QString baseUrl = Settings::instance()->serverIp;
    if (baseUrl.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "serverIp:" << baseUrl;

    QString owner = Settings::instance()->usernameEn;
    if (owner.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "owner:" << owner;
    QString account = Settings::instance()->userAccount;
    if (account.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "account:" << account;
    QString password = Settings::instance()->userPassword;
    if (password.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "password:" << password;

#ifdef Q_OS_WIN
    QString typoraPath = Settings::instance()->typoraPath;
    if (typoraPath.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "typoraPath:" << typoraPath;
#endif
    if (needSetConfig) {
        auto settingsDialog = new SettingsDialog();
        auto ret = settingsDialog->exec();
        if (ret == QDialog::Accepted) {
            return showWindow(&a);
        }
    } else {
        return showWindow(&a);
    }
    return 0;
}

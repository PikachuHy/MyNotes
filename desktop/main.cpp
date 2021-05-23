#include <qglobal.h>
#include "Widget.h"
#include "SettingsDialog.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "SingleApplication.h"
#include <QDebug>
#ifndef _DEBUG
#include <QLoggingCategory>
#include <Logger.h>
#include <ConsoleAppender.h>
#include <FileAppender.h>
#endif
#include <QCommandLineParser>
#include "config.h"
#include "LoginDialog.h"
#include "Utils.h"
int showWindow(SingleApplication* app) {
    auto w = new Widget();
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    QObject::connect(app, &SingleApplication::messageAvailable,
        [w](const QString& message) {
        qDebug() << "show";
        w->showNormal();
    }
    );
    w->show();
    auto ret = QApplication::exec();
    if (ret != 0) {
        qWarning() << "Something went wrong." << "Result code is" << ret;
    }
    return ret;
}
int main(int argc, char *argv[]) {
    // 不是在dll中使用的话，默认是找不到这些资源的
    Q_INIT_RESOURCE(css);
    Q_INIT_RESOURCE(db);
    Q_INIT_RESOURCE(icon);
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0) && QT_VERSION < QT_VERSION_CHECK(6,0,0))
    QSettings settings(
            QString("%1/PikachuHy/MyNotes/config.ini")
                    .arg(QStandardPaths::standardLocations(
                            QStandardPaths::ConfigLocation).first()
                    ),
            QSettings::IniFormat);
    if (settings.value(Settings::KEY_MODE_HIDPI, false).toBool()) {
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        qInfo() << "enable HiDPI";
    }
#else
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
    SingleApplication a(argc, argv, "MyNotes");
    if (a.isRunning()) {
        a.sendMessage("app in running");
        return 0;
    }
#ifdef _DEBUG
     qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9223");
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
#ifndef _DEBUG
    if (parser.isSet(debugOption)) {
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
        qDebug() << "run with debug mode";
    }
#endif
    bool needLogin = false;
    bool autoLogin = Settings::instance()->userAutoLogin;
    if (!autoLogin) {
        needLogin = true;
    }
    qDebug() << "auto login: " << autoLogin;
    QString account = Settings::instance()->userAccount;
    if (account.isEmpty()) {
        needLogin = true;
    }
    qDebug() << "account:" << account;
    QString password = Settings::instance()->userPassword;
    if (password.isEmpty()) {
        needLogin = true;
    }
    qDebug() << "password:" << password;
    QString usernameZh = Settings::instance()->usernameZh;
    QString usernameEn = Settings::instance()->usernameEn;
    if (usernameZh.isEmpty() || usernameEn.isEmpty()) {
        needLogin = true;
    }
    qDebug() << usernameZh << usernameEn;
    QString signature = Settings::instance()->userSignature;
    if (signature.isEmpty()) {
        needLogin = true;
    } else {
        QString infoStr = account + '.' + password + '.'
                + usernameZh + '.' + usernameEn + ".MyNotes";
        QString md5 = Utils::md5(infoStr);
        if (signature != md5) {
            needLogin = true;
        }
    }
    qDebug() << "signature:" << signature;
#ifdef Q_OS_ANDROID
#else
    if (needLogin && !Settings::instance()->modeOffline) {
        LoginDialog dialog;
        auto ret = dialog.exec();
        if (ret == QDialog::Rejected) {
            qInfo() << "login fail";
            return 0;
        }
    }
#endif
    bool needSetConfig = false;
    QString serverIp = Settings::instance()->serverIp;
    if (serverIp.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "serverIp:" << serverIp;
#ifdef Q_OS_WIN
    QString typoraPath = Settings::instance()->typoraPath;
    if (typoraPath.isEmpty()) {
        needSetConfig = true;
    }
    qDebug() << "typoraPath:" << typoraPath;
#endif
#ifdef Q_OS_ANDROID
    return showWindow(&a);
#else
    if (needSetConfig) {
        auto settingsDialog = new SettingsDialog();
        auto ret = settingsDialog->exec();
        if (ret == QDialog::Accepted) {
            return showWindow(&a);
        }
    } else {
        return showWindow(&a);
    }
#endif
    return 0;
}

//
// Created by pikachu on 2021/10/10.
//
#include <qglobal.h>
#include "MainWindow.h"
#include "Widget.h"
#include "SettingsDialog.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "SingleApplication.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "editor/QtQuickMarkdownEditor.h"
#include "controller/Controller.h"
#include "controller/NoteController.h"
#include "controller/Clipboard.h"
#include "controller/SettingDialogController.h"
#include "controller/KeyFilter.h"
#include "controller/FileSystem.h"
#include "BeanFactory.h"
#include <QFileSystemWatcher>
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
#include "DbManager.h"
#include "TreeModel.h"

#include "control/TreeModelAdaptor.h"
#include <QQuickStyle>

#ifdef Q_OS_ANDROID
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtAndroidExtras>
#else
#include <private/qandroidextras_p.h>
#include <QFuture>
#include <QJniObject>
#endif
#endif

#include <QFontDatabase>
#if 1
#include <QtWebView>
#endif
#include <QQuickWindow>

// #include <QtWebEngineQuick>
using md::editor::QtQuickMarkdownEditor;

void showQtQuickVersion(QApplication *app) {
  QQuickStyle::setStyle("Material");
  //    QQuickStyle::setStyle("Universal");
  //    QQuickStyle::setStyle("macOS");
  auto fileSystemWatcher = new QFileSystemWatcher();
  BeanFactory::instance()->registerBean("fileSystemWatcher", fileSystemWatcher);
  auto controller = new Controller();
  auto notesPath = controller->noteDataPath();
  auto dbManager = new DbManager(notesPath);
  BeanFactory::instance()->registerBean("dbManager", dbManager);
  auto treeModel = new TreeModel(notesPath, dbManager);
  BeanFactory::instance()->registerBean("treeModel", treeModel);

  auto keyFilter = new KeyFilter();
  auto fs = new FileSystem();

  auto engine = new QQmlApplicationEngine();
  qmlRegisterType<QtQuickMarkdownEditor>("QtMarkdown", 1, 0, "QtQuickMarkdownEditor");
  //    qmlRegisterType<TreeModelAdaptor>("cn.net.pikachu.control", 1, 0, "TreeModelAdaptor");
  qmlRegisterType<TreeModelAdaptor>("cn.net.pikachu.control", 1, 0, "TreeModelAdaptor");
  qmlRegisterType<Controller>("Controller", 1, 0, "Controller");
  qmlRegisterType<NoteController>("Controller", 1, 0, "NoteController");
  qmlRegisterType<SettingDialogController>("Controller", 1, 0, "SettingDialogController");


  engine->rootContext()->setContextProperty("$Controller",controller);
  engine->rootContext()->setContextProperty("$Model",treeModel);
  engine->rootContext()->setContextProperty("$DbManager", dbManager);
  engine->rootContext()->setContextProperty("$KeyFilter", keyFilter);
  engine->rootContext()->setContextProperty("$FileSystem", fs);

  engine->rootContext()->setContextProperty("treeModel", treeModel);
  engine->rootContext()->setContextProperty("clipboard", new Clipboard());
  QUrl url;
#ifdef Q_OS_ANDROID
  url = QStringLiteral("qrc:/mobile_main.qml");
#else
  if (Settings::instance()->modeRender == 3) {
      url = QStringLiteral("qrc:/desktop_main.qml");
  } else {
      url = QStringLiteral("qrc:/mobile_main.qml");
  }

#endif
#ifdef Q_OS_ANDROID
#if 0
  auto activity = QtAndroidPrivate::activity();
    if (activity->isValid()) {
        auto intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
        if (intent.isValid()) {
            QAndroidJniObject data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
            if (data.isValid()) {
                QAndroidJniObject path = data.callObjectMethod("getPath", "()Ljava/lang/String;");
                if (path.isValid())
                    // Here path.toString() returns the path of the input file
                    QMetaObject::invokeMethod(rootComponent, "setSourcePath", Q_ARG(QVariant, QVariant("file://" + path.toString())));
            }
        }
    }
#endif
#endif
  QObject::connect(engine, &QQmlApplicationEngine::objectCreated,
      app, [url, keyFilter](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
          qCritical() << "load error:" << url;
          QCoreApplication::exit(-1);
        }
        // 拦截安卓返回键
        keyFilter->setFilter(obj);
      }, Qt::QueuedConnection);

  engine->load(url);
}
int showWindow(SingleApplication *app) {
#ifdef Q_OS_ANDROID
  int modeRender = 3;
#else
  int modeRender = Settings::instance()->modeRender;
#endif
  qDebug() << "render mode:" << modeRender;
  if (modeRender == 3 || modeRender == 4) {
    showQtQuickVersion(app);
  } else {
    auto w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    QObject::connect(app, &SingleApplication::messageAvailable,
                     [w](const QString &message) {
                       qDebug() << "show";
                       w->showNormal();
                     }
    );
    w->show();
  }
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
#ifndef Q_OS_WIN
  Q_INIT_RESOURCE(md);
#endif
  Q_INIT_RESOURCE(qml);
#if 1
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
#endif
  QtWebView::initialize();
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0) && QT_VERSION < QT_VERSION_CHECK(6,0,0))
  QString configPath = QString("%1/PikachuHy/MyNotes/config.ini")
                           .arg(QStandardPaths::standardLocations(
                                    QStandardPaths::ConfigLocation).first()
                           );
  qDebug() << "config path:" << configPath;
  QSettings settings(
      configPath,
      QSettings::IniFormat);
  if (settings.value(Settings::KEY_MODE_HIDPI, false).toBool()) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qInfo() << "enable HiDPI";
  }
#else
//  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
  SingleApplication a(argc, argv, "MyNotes");
  if (a.isRunning()) {
    a.sendMessage("app in running");
    return 0;
  }
#ifdef Q_OS_ANDROID
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QString needPermission = QString("android.permission.WRITE_EXTERNAL_STORAGE");
  auto result = QtAndroid::checkPermission(needPermission);
  if(result == QtAndroid::PermissionResult::Denied){
    QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({needPermission}));
    if(resultHash[needPermission] == QtAndroid::PermissionResult::Denied) {
      qCritical() << "no permission:" << needPermission;
      return 0;
    }
  }
#else
  using namespace QtAndroidPrivate;

  auto perm = QString("android.permission.WRITE_EXTERNAL_STORAGE");
  auto qtGetAndroidPerm = [](QString perm) {
      auto result = checkPermission(perm);
      if(result.result() == PermissionResult::Denied){
        result = requestPermission(perm);
        if (result.result() == PermissionResult::Denied) {
          qDebug() << "no permission: " << perm;
        } else if (result.result() == PermissionResult::Authorized) {
            qDebug() << "auto permission: " << perm;
        }
      }
  };
  qtGetAndroidPerm(perm);
  perm = QString("android.permission.MANAGE_EXTERNAL_STORAGE");
  qtGetAndroidPerm(perm);

#endif
#endif
#if 0
  qDebug() << a.font();
  QFontDatabase database;
  const QStringList fontFamilies = database.families();
  qDebug() << fontFamilies;
#endif
  QFont font;
#ifdef Q_OS_MAC
  font.setFamily(".AppleSystemUIFont");
#endif
#ifdef Q_OS_WIN
  font.setFamily("微软雅黑");
#else
  font.setFamily("Noto Sans CJK SC");
#endif
  a.setFont(font);
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
  qDebug() << QApplication::font();
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
  auto args = parser.positionalArguments();
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

//
// Created by pikachu on 5/22/2021.
//

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

#include "QtQuickMarkdownItem.h"
#include "Controller.h"
#include "DbManager.h"
#include "TreeModel.h"
#include "KeyFilter.h"
#include "FileSystem.h"
#ifdef Q_OS_ANDROID
#include <private/qandroidextras_p.h>
#include <QFuture>
#endif
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(css);
    Q_INIT_RESOURCE(db);
    Q_INIT_RESOURCE(icon);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
#ifdef Q_OS_ANDROID
    using namespace QtAndroidPrivate;
    auto result = checkPermission(QString("android.permission.WRITE_EXTERNAL_STORAGE"));
    if(result.result() == PermissionResult::Denied){
        result = requestPermission(QString("android.permission.WRITE_EXTERNAL_STORAGE"));
        if (result.result() == PermissionResult::Denied) {
            qDebug() << "no permission";
            qApp->exit(0);
        }
    }
#endif
    auto controller = new Controller();
    auto notesPath = controller->noteDataPath();
    qDebug() << "note path: " << notesPath;
    auto dbManager = new DbManager(notesPath);
    auto treeModel = new TreeModel(notesPath, dbManager);
    auto keyFilter = new KeyFilter();
    auto fs = new FileSystem();
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("$Controller",controller);
    engine.rootContext()->setContextProperty("$Model",treeModel);
    engine.rootContext()->setContextProperty("$DbManager", dbManager);
    engine.rootContext()->setContextProperty("$KeyFilter", keyFilter);
    engine.rootContext()->setContextProperty("$FileSystem", fs);
    qmlRegisterType<QtQuickMarkdownItem>("cn.net.pikachu.control", 1, 0, "QtQuickMarkdownItem");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url, keyFilter](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
                keyFilter->setFilter(obj);
            }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}

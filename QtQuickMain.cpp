//
// Created by pikachu on 5/22/2021.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

#include "QtQuickMarkdownItem.h"
#include "DbManager.h"
#include "TreeModel.h"
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(css);
    Q_INIT_RESOURCE(db);
    Q_INIT_RESOURCE(icon);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    auto notesPath = docPath + "/MyNotes/";
    if (!QFile(notesPath).exists()) {
        qDebug() << "mkdir" << notesPath;
        QDir().mkdir(notesPath);
    }
    auto dbManager = new DbManager(notesPath);
    auto treeModel = new TreeModel(notesPath, dbManager);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("$Model",treeModel);
    engine.rootContext()->setContextProperty("$DbManager",dbManager);
//    qmlRegisterType<QtQuickMarkdownItem>("cn.net.pikachu.control", 1, 0, "QtQuickMarkdownItem");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}

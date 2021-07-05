//
// Created by pikachu on 2021/6/14.
//

#include "Controller.h"
#include "TreeItem.h"
#include "Settings.h"
#include "TreeItem.h"
#include "BeanFactory.h"

#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QFileSystemWatcher>

Controller::Controller(QObject *parent) : QObject(parent) {
    auto fileSystemWatcher = BeanFactory::instance()->getBean<QFileSystemWatcher>("fileSystemWatcher");
    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path){
        emit noteChanged(path);
    });
}


QString Controller::lastOpenedNote() {
    return Settings::instance()->lastOpenNotePath;
}

void Controller::setLastOpenedNote(QString path) {
    Settings::instance()->lastOpenNotePath = path;
}

void Controller::openUrl(QString url) {
    QDesktopServices::openUrl(QUrl(url));
}

QRect Controller::lastWindowRect() {
    QRect rect = Settings::instance()->mainWindowGeometry;
    qDebug() << "last window size:" << rect;
    return rect;
}

void Controller::setLastWindowRect(QRect rect) {
    Settings::instance()->mainWindowGeometry = rect;
}

void Controller::watchNote(const QString &path) {
    auto fileSystemWatcher = BeanFactory::instance()->getBean<QFileSystemWatcher>("fileSystemWatcher");
    bool ret = fileSystemWatcher->addPath(path);
    if (!ret) {
        qWarning() << "fileSystemWatcher.addPath error: " << path;
    }
}

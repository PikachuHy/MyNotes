//
// Created by pikachu on 2021/10/5.
//

#include "FileSystem.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

QStringList FileSystem::listDir(QString path) {
    qDebug() << "path: " << path;
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        return QDir(path).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    }
    return {};
}

bool FileSystem::isDir(QString path) {
    return QFileInfo(path).isDir();
}

bool FileSystem::isFile(QString path) {
    return QFileInfo(path).isFile();
}

QString FileSystem::defaultPath() {
#ifdef Q_OS_ANDROID
    return "/storage/emulated/0";
#else
    return QDir::homePath();
#endif
}

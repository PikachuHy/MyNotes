//
// Created by pikachu on 2021/6/14.
//

#include "Controller.h"
#include "TreeItem.h"
#include "Settings.h"
#include "BeanFactory.h"

#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QFile>
#include <QDir>

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

QString Controller::notePathFromModelIndex(QModelIndex index)
{
    if (!index.isValid()) {
        qDebug() << "invalid index";
        return "";
    }
    auto item = (NoteItem*)index.internalPointer();
    if (!item) {
        qDebug() << "(NoteItem*) error";
        return "";
    }
    return QString("%1/workshop/%2/index.md").arg(noteDataPath(), item->path());
}

bool Controller::isNoteItem(QModelIndex index)
{
    if (!index.isValid()) {
        qDebug() << "invalid index";
        return "";
    }

    auto item = (TreeItem*)index.internalPointer();
    if (!item) {
        qDebug() << "(TreeItem*) error";
        return "";
    }
    return item->isNoteItem();
}


QString Controller::noteDataPath() {
    QString path = Settings::instance()->noteDataPath;
    if (path.isEmpty()) {
        auto docPaths = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        QString docPath;
        if (docPaths.isEmpty()) {
            qCritical() << "no documents path";
            docPath = QDir::homePath();
        } else {
            docPath = docPaths.first();
        }
        auto notesPath = docPath + "/MyNotes/";
        if (!QFile(notesPath).exists()) {
            qDebug() << "mkdir" << notesPath;
            QDir().mkdir(notesPath);
        }
        return notesPath;
    }
    return path;
}

void Controller::setNoteDataPath(QString path) {
    Settings::instance()->noteDataPath = path;
}

QString Controller::configStorePath() {
    return Settings::instance()->configStorePath();
}

//
// Created by pikachu on 2021/10/5.
//

#include "Controller.h"
#include "Settings.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>


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

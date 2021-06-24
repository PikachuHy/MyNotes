//
// Created by pikachu on 2021/6/14.
//

#include "Controller.h"
#include "TreeItem.h"
#include "Settings.h"
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
bool Controller::isNote(QModelIndex index) {
    if (!index.isValid()) {
        return false;
    }
    auto item =(TreeItem*) index.internalPointer();
    if (item == nullptr) {
        return false;
    }
    return item->isNoteItem();
}

QString Controller::getNoteFullPath(QModelIndex index) {
    if (!index.isValid()) {
        return QString();
    }

    auto item =(NoteItem*) index.internalPointer();
    if (item == nullptr) {
        return QString();
    }

    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    auto notePath = docPath + "/MyNotes/workshop/" + item->path() + "/index.md";
    return notePath;
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

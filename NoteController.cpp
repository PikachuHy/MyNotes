//
// Created by pikachu on 2021/6/24.
//

#include "NoteController.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QModelIndex>

#include "DbManager.h"
#include "DbModel.h"
#include "Utils.h"
#include "BeanFactory.h"
#include "TreeItem.h"
#include "TreeModel.h"

QString noteRealPath(const Note& note) {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    return QString("%1/MyNotes/workshop/%2/index.md").arg(docPath, note.strId());
}

NoteController::NoteController(QObject *parent) : QObject(parent) {

}

QModelIndex NoteController::createNewNote(QModelIndex index, QString noteName) {
    if (!index.isValid()) {
        return QModelIndex();
    }
    auto item =(TreeItem*) index.internalPointer();
    if (item == nullptr) {
        qDebug() << "current item is nullptr";
        return QModelIndex();
    }
    int pathId = item->pathId();
    auto strId = Utils::generateId();
    Note note(strId, noteName, pathId);
    QString newNotePath = noteRealPath(note);
    QDir dir;
    dir.mkpath(QFileInfo(newNotePath).path());
    QFile file = QFile(newNotePath);

    qDebug() << "create new note:" << noteName << "in" << newNotePath;
    auto ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        qDebug() << "open file fail:" << newNotePath;
        return QModelIndex();
    }
    auto m_dbManager = BeanFactory::instance()->getBean<DbManager>("dbManager");
    m_dbManager->addNewNote(note);
    TreeItem* parentItem = item->isFile() ? item->parentItem() : item;
    auto noteItem = new NoteItem(note, parentItem);
    item->setPath(newNotePath);
    auto m_treeModel = BeanFactory::instance()->getBean<TreeModel>("treeModel");
    auto newNoteIndex = m_treeModel->addNewNode(index, noteItem);
//    m_treeView->setCurrentIndex(newNoteIndex);
    QString newNoteText = "# " + noteName;
    file.write(newNoteText.toUtf8());
    file.close();
    return newNoteIndex;
}


bool NoteController::isNote(QModelIndex index) {
    if (!index.isValid()) {
        return false;
    }
    auto item =(TreeItem*) index.internalPointer();
    if (item == nullptr) {
        return false;
    }
    return item->isNoteItem();
}

QString NoteController::getNoteFullPath(QModelIndex index) {
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

int NoteController::getPathId(QModelIndex index) {
    if (!index.isValid()) {
        return 0;
    }

    auto item =(NoteItem*) index.internalPointer();
    if (item == nullptr) {
        return 0;
    }
    return item->pathId();
}

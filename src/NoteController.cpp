//
// Created by pikachu on 2021/6/24.
//

#include "NoteController.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QModelIndex>
#include <QProcess>

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

inline QString workshopPath() {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    return QString("%1/MyNotes/workshop/").arg(docPath);
}
inline QString tmpPath() {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    return QString("%1/MyNotes/tmp/").arg(docPath);
}
inline QString trashPath() {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    return QString("%1/MyNotes/trash/").arg(docPath);
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

void NoteController::trashNote(QModelIndex index) {
    if (!index.isValid()) {
        qDebug() << "index is invalid";
        return;
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    auto m_dbManager = BeanFactory::instance()->getBean<DbManager>("dbManager");
    int noteId = item->note().id();
    qDebug() << "trash note id:" << noteId;
    auto ret = m_dbManager->removeNote(noteId);
    if (!ret) {
        qDebug() << "trash note fail";
        return;
    }
    const QString noteOldPath = workshopPath() + item->note().strId();
    QString noteTrashPath = trashPath() + item->note().strId();
    qDebug() << "trash" << noteOldPath << "to" << noteTrashPath;
    ret = QDir().rename(noteOldPath, noteTrashPath);
    if (!ret) {
        qDebug() << "trash" << noteOldPath << "fail";
        return;
    }
    auto m_treeModel = BeanFactory::instance()->getBean<TreeModel>("treeModel");
    m_treeModel->removeNode(index);
}

QModelIndex NoteController::createNewFolder(QModelIndex index, QString folderName) {

    if (!index.isValid()) {
        qDebug() << "index is invalid";
        return QModelIndex();
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    auto m_dbManager = BeanFactory::instance()->getBean<DbManager>("dbManager");
    auto ret = m_dbManager->isPathExist(folderName, item->pathId());
    if (ret) {
        qDebug() << "path exist." << folderName;
        return QModelIndex();
    }
    Path path(folderName, item->pathId());
    ret = m_dbManager->addNewPath(path);
    if (!ret) {
        qDebug() << "save to db fail";
        return QModelIndex();
    }
    auto newPathItem = new FolderItem(path, item);
    auto m_treeModel = BeanFactory::instance()->getBean<TreeModel>("treeModel");
    auto newNodeIndex = m_treeModel->addNewFolder(index, newPathItem);
    return newNodeIndex;
}

void NoteController::trashFolder(QModelIndex index) {
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<FolderItem *>(index.internalPointer());
    auto m_dbManager = BeanFactory::instance()->getBean<DbManager>("dbManager");
    auto ret = m_dbManager->removePath(item->path().id());
    if (!ret) {
        qDebug() << "trash path fail";
        return;
    }
    auto m_treeModel = BeanFactory::instance()->getBean<TreeModel>("treeModel");
    m_treeModel->removeNode(index);
    qDebug() << "trash" << item->path().name();
}

void NoteController::openInTypora(const QString &path) {
    qDebug() << "open in typora:" << path;
#ifdef Q_OS_WIN
    QStringList pathList;
    QString typoraPath = Settings::instance()->typoraPath;
    pathList << typoraPath;
    pathList << "C:\\Program Files\\Typora\\Typora.exe";
    pathList << "C:\\Program Files (x86)\\Typora\\Typora.exe";
    pathList << "D:\\typora\\Typora\\Typora.exe";
    QString exePath;
    bool exeFind = false;
    for (auto candidatePath: pathList) {
        if (QFile(candidatePath).exists()) {
            exeFind = true;
            exePath = candidatePath;
        }
    }
    if (!exeFind) {
        qWarning() << "Please install Typora first.";
    } else {
        QStringList cmd;
        cmd << path;
        QProcess::startDetached(exePath, cmd);
    }
#else
    QStringList cmd;
    cmd << "-a" << "typora" << path;
    QProcess::startDetached("open",cmd);
#endif
}

#include "NoteFileService.h"
#include "DbManager.h"
#include "Utils.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

NoteFileService::NoteFileService(DbManager* dbManager, const QString& workshopPath,
                                 const QString& trashPath, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
    , m_workshopPath(workshopPath)
    , m_trashPath(trashPath)
{
}

Note NoteFileService::createNote(const QString& name, int parentPathId)
{
    auto strId = Utils::generateId();
    Note note(strId, name, parentPathId);
    QString newNotePath = noteRealPath(note);
    QDir dir;
    dir.mkpath(QFileInfo(newNotePath).path());
    QFile file(newNotePath);
    bool ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        qDebug() << "open file fail:" << newNotePath;
        return Note();
    }
    m_dbManager->addNewNote(note);
    QString newNoteText = "# " + name;
    file.write(newNoteText.toUtf8());
    file.close();
    return note;
}

Path NoteFileService::createFolder(const QString& name, int parentId)
{
    Path path(name, parentId);
    m_dbManager->addNewPath(path);
    return path;
}

bool NoteFileService::trashNote(const Note& note)
{
    auto ret = m_dbManager->removeNote(note.id());
    if (!ret) {
        qDebug() << "trash note fail";
        return false;
    }
    const QString noteOldPath = m_workshopPath + note.strId();
    QString noteTrashPath = m_trashPath + note.strId();
    qDebug() << "trash" << noteOldPath << "to" << noteTrashPath;
    ret = QDir().rename(noteOldPath, noteTrashPath);
    if (!ret) {
        qDebug() << "trash" << noteOldPath << "fail";
        return false;
    }
    return true;
}

bool NoteFileService::trashFolder(const Path& path)
{
    auto ret = m_dbManager->removePath(path.id());
    if (!ret) {
        qDebug() << "trash path fail";
        return false;
    }
    return true;
}

QString NoteFileService::noteRealPath(const Note& note) const
{
    return m_workshopPath + note.strId() + "/index.md";
}

Note NoteFileService::getNote(int id)
{
    return m_dbManager->getNote(id);
}

Note NoteFileService::getNote(const QString& strId)
{
    return m_dbManager->getNote(strId);
}

Path NoteFileService::getPath(int id)
{
    return m_dbManager->getPath(id);
}

bool NoteFileService::isPathExist(const QString& name, int parentId)
{
    return m_dbManager->isPathExist(name, parentId);
}

QString NoteFileService::noteStrIdFromWorkshopPath(const QString& path) const
{
    QStringList segs = path.split('/');
    QString noteStrId = segs[segs.size() - 2];
    return noteStrId;
}

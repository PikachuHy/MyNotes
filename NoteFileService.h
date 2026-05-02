#ifndef MYNOTES_NOTEFILESERVICE_H
#define MYNOTES_NOTEFILESERVICE_H

#include <QObject>
#include <QString>
#include "database/DbModel.h"

class DbManager;

class NoteFileService : public QObject {
    Q_OBJECT
public:
    explicit NoteFileService(DbManager* dbManager, const QString& workshopPath,
                             const QString& trashPath, QObject *parent = nullptr);

    Note createNote(const QString& name, int parentPathId);
    Path createFolder(const QString& name, int parentId);
    bool trashNote(const Note& note);
    bool trashFolder(const Path& path);

    QString noteRealPath(const Note& note) const;
    Note getNote(int id);
    Note getNote(const QString& strId);
    Path getPath(int id);
    bool isPathExist(const QString& name, int parentId);
    QString noteStrIdFromWorkshopPath(const QString& path) const;

signals:
    void noteCreated(const Note& note);
    void folderCreated(const Path& path);

private:
    DbManager* m_dbManager;
    QString m_workshopPath;
    QString m_trashPath;
};

#endif

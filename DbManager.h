//
// Created by PikachuHy on 2020/12/26.
//

#ifndef MYNOTES_DBMANAGER_H
#define MYNOTES_DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "DbModel.h"
class DbManager : public QObject {
Q_OBJECT
public:
    explicit DbManager(const QString &dataPath, QObject *parent = nullptr);

    virtual ~DbManager();

    QList<Path> getPathList(int parentPathId = 0);
    Q_INVOKABLE QVariantList getPathList_qml(int parentPathId = 0);
    QList<Note> getNoteList(int pathId = 0);
    Q_INVOKABLE QVariantList getNoteList_qml(int pathId = 0);
    QList<Note> getNoteList(QStringList words);
    QList<Note> getAllNotes();
    bool addNewNote(Note & note);
    bool addNewPath(Path & path);
    bool removeNote(int id);
    bool removePath(int id);
    Note getNote(int id);
    Note getNote(const QString& strId);
    Path getPath(int id);
    bool isPathExist(QString name, int parentId);

    bool updateIndex(QStringList wordList, int id);

private:
    bool execDbSetupSql(QString path);
    bool execSql(QSqlQuery& query, bool batch = false);
    void fillNote(Note& note, QSqlQuery& query);
    void fillPath(Path & path, QSqlQuery& query);
    inline QSqlQuery _query();
    inline QSqlQuery _query(QString sql);
private:
    QSqlDatabase db;
    QString m_connectionName;
};


#endif //MYNOTES_DBMANAGER_H

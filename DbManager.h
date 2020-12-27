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
    QList<Note> getNoteList(int pathId = 0);
    bool addNewNote(Note & note);
    bool addNewPath(Path & path);
    bool removeNote(int id);
    bool removePath(int id);
    Note getNote(int id);
    Path getPath(int id);
    bool isPathExist(QString name, int parentId);

    bool updateIndex(QStringList wordList, int id);
private:
    bool execDbSetupSql(QString path);
    bool execSql(QSqlQuery& query, bool batch = false);
    void fillNote(Note& note, QSqlQuery& query);
    void fillPath(Path & path, QSqlQuery& query);
private:
    QSqlDatabase db;
};


#endif //MYNOTES_DBMANAGER_H

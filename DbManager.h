//
// Created by PikachuHy on 2020/12/26.
//

#ifndef MYNOTES_DBMANAGER_H
#define MYNOTES_DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
class DbManager;
class DbModel {
public:
    inline int id() const { return m_id; };

private:
    int m_id;
    int m_trashed;
    int m_createTime;
    int m_updateTime;
    friend DbManager;
};

class Note : public DbModel {
public:
    inline QString strId() const { return m_strId; }
    inline QString title() const { return m_title; }
    inline int pathId() const { return m_pathId; }
private:
    QString m_strId;
    QString m_title;
    int m_pathId;
    friend DbManager;
};

class Path : public DbModel {
public:
    inline QString name() const { return m_name; }
private:
    QString m_name;
    friend DbManager;
};

class DbManager : public QObject {
Q_OBJECT
public:
    explicit DbManager(const QString &dataPath, QObject *parent = nullptr);

    virtual ~DbManager();

    QList<Path> getPathList(int parentPathId = 0);
    QList<Note> getNoteList(int pathId = 0);
private:
    bool execDbSetupSql(QString path);

private:
    QSqlDatabase db;
};


#endif //MYNOTES_DBMANAGER_H

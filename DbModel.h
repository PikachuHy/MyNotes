//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_DBMODEL_H
#define MYNOTES_DBMODEL_H
#include <QString>
class DbManager;
class DbModel {
public:
    inline int id() const { return m_id; }

private:
    int m_id;
    int m_trashed;
    int m_createTime;
    int m_updateTime;
    friend DbManager;
};

class Note : public DbModel {
public:
    Note();
    Note(const QString &strId, const QString &title, int pathId);

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



#endif //MYNOTES_DBMODEL_H

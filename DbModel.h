//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_DBMODEL_H
#define MYNOTES_DBMODEL_H
#include <QString>
#include <QVariant>
class DbManager;
class DbModel {
public:
    inline int id() const { return m_id; }
    inline bool trashed() const { return m_trashed; }
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
    int m_security;
    friend DbManager;
};
Q_DECLARE_METATYPE(Note)
class Path : public DbModel {
public:
    Path() {}
    Path(const QString &name, int parentId);

    inline QString name() const { return m_name; }
    inline int parentId() const { return m_parentId; }
private:
    QString m_name;
    int m_parentId;
    int m_security;
    friend DbManager;
};



#endif //MYNOTES_DBMODEL_H

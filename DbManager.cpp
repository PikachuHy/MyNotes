//
// Created by PikachuHy on 2020/12/26.
//

#include "DbManager.h"
#include <QtCore>
#include <QtWidgets>
#include <QtSql>
DbManager::DbManager(const QString& dataPath, QObject *parent): QObject(parent) {

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataPath+"db/sqlite.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, tr("Cannot open database"),
                              tr("Unable to establish a database connection.\n"
                                 "This example needs SQLite support. Please read "
                                 "the Qt SQL driver documentation for information how "
                                 "to build it."), QMessageBox::Cancel);
        return;
    }
    execDbSetupSql(":/sql/create_table_note.sql");
    execDbSetupSql(":/sql/create_trigger_note_auto_update_time.sql");
    execDbSetupSql(":/sql/create_table_path.sql");
    execDbSetupSql(":/sql/create_trigger_path_auto_update_time.sql");
}

bool DbManager::execDbSetupSql(QString path) {

    QFile file(path);
    if (!file.exists()) {
        qDebug() << "ERROR:"<< path <<"not exist";
        return false;
    }
    file.open(QIODevice::ReadOnly);
    QString sql = file.readAll();
    QSqlQuery query;
    bool ret = query.exec(sql);
    if (!ret) {
        qDebug() << "exec sql fail:";
        qDebug().nospace().noquote() << sql;
        auto w = new QDialog();
        auto vbox = new QVBoxLayout();
        vbox->addWidget(new QLabel("ERROR in exec sql:"));
        auto t = new QTextBrowser();
        t->setText(sql);
        vbox->addWidget(t);
        w->setLayout(vbox);
        w->exec();
        connect(w, &QLabel::destroyed, w, &QLabel::deleteLater);
        return false;
    }
    file.close();
    return true;
}

QList<Path> DbManager::getPathList(int parentPathId) {
    QList<Path> ret;
    QSqlQuery query("select * from path where parent_id = " + QString::number(parentPathId));
    while (query.next()) {
        Path path;
        fillPath(path, query);
        ret.append(path);
    }
    return ret;
}

QList<Note> DbManager::getNoteList(int pathId) {
    QList<Note> ret;
    QSqlQuery query("select * from note where path = " + QString::number(pathId));
    while (query.next()) {
        Note note;
        fillNote(note, query);
        ret.append(note);
    }
    return ret;
}

DbManager::~DbManager() {
    db.close();
}

bool DbManager::addNewNote(Note &note) {
    QSqlQuery query;
    query.prepare("insert into note (str_id, title, path) values (:str_id, :title, :path)");
    query.bindValue(":str_id", note.strId());
    query.bindValue(":title", note.title());
    query.bindValue(":path", note.pathId());
    auto ret = query.exec();
    if (!ret) {
        qDebug() << "exec sql fail: " << query.lastQuery();
        qDebug() << query.lastError().text();
        return false;
    }
    int id = query.lastInsertId().toInt();
    note = getNote(id);
    return true;
}

Note DbManager::getNote(int id) {
    Note note;
    QSqlQuery query("select * from note where id = " + QString::number(id));
    while (query.next()) {
        fillNote(note, query);
    }
    return note;
}

void DbManager::fillNote(Note &note, QSqlQuery &query) {
    note.m_id = query.value("id").toInt();
    note.m_strId = query.value("str_id").toString();
    note.m_trashed = query.value("trashed").toInt();
    note.m_title = query.value("title").toString();
    note.m_pathId = query.value("path").toInt();
    note.m_createTime = query.value("create_time").toInt();
    note.m_updateTime = query.value("update_time").toInt();
}

bool DbManager::addNewPath(Path &path) {
    QSqlQuery query;
    query.prepare("insert into path (name, parent_id) values (:name, :parent_id)");
    query.bindValue(":name", path.name());
    query.bindValue(":parent_id", path.parentId());
    auto ret = query.exec();
    if (!ret) {
        qDebug() << "exec sql fail: " << query.lastQuery();
        qDebug() << query.lastError().text();
        return false;
    }
    int id = query.lastInsertId().toInt();
    path = getPath(id);
    return true;
}

Path DbManager::getPath(int id) {
    Path path;
    QSqlQuery query("select * from path where id = " + QString::number(id));
    while (query.next()) {
        fillPath(path, query);
    }
    return path;
}

void DbManager::fillPath(Path &path, QSqlQuery &query) {
    path.m_id = query.value("id").toInt();
    path.m_parentId = query.value("parent_id").toInt();
    path.m_trashed = query.value("trashed").toInt();
    path.m_name = query.value("name").toString();
    path.m_createTime = query.value("create_time").toInt();
    path.m_updateTime = query.value("update_time").toInt();
}

bool DbManager::isPathExist(QString name, int parentId) {
    QSqlQuery query("select * from path where parent_id = " + QString::number(parentId) + " and name = " + name);
    while (query.next()) {
        return true;
    }
    return false;
}

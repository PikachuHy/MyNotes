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
        path.m_id = query.value("id").toInt();
        path.m_trashed = query.value("trashed").toInt();
        path.m_name = query.value("name").toString();
        path.m_createTime = query.value("create_time").toInt();
        path.m_updateTime = query.value("update_time").toInt();
        ret.append(path);
    }
    return ret;
}

QList<Note> DbManager::getNoteList(int pathId) {
    QList<Note> ret;
    QSqlQuery query("select * from note where path = " + QString::number(pathId));
    while (query.next()) {
        Note note;
        note.m_id = query.value("id").toInt();
        note.m_trashed = query.value("trashed").toInt();
        note.m_title = query.value("title").toString();
        note.m_pathId = query.value("path").toInt();
        note.m_createTime = query.value("create_time").toInt();
        note.m_updateTime = query.value("update_time").toInt();
        ret.append(note);
    }
    return ret;
}

DbManager::~DbManager() {
    db.close();
}

//
// Created by PikachuHy on 2020/12/26.
//

#include "DbManager.h"
#include "Constant.h"
#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QThread>
#include <QSqlError>
DbManager::DbManager(const QString& dataPath, QObject *parent): QObject(parent) {
    QString threadId = QString::number((long long)QThread::currentThread(), 16);
    m_connectionName = "db_connect_" + threadId;
    db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
//    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = dataPath+"/db";
    if (!QDir(dbPath).exists()) {
        qDebug() << "mkdir" << dbPath;
        QDir().mkdir(dbPath);
    }
    QString database = dataPath+"/db/sqlite.db";
    if (QFile(database).exists()) {
        qDebug() << "file exist";
    } else {
        qDebug() << "file not exist";
    }
    qDebug() << "database:" << database;
    db.setDatabaseName(database);
    if (!db.open()) {
        auto err = db.lastError();
        qDebug() << "db err:" << err;

        QString errMsg = tr("Unable to establish a database connection.\n"
                                 "This example needs SQLite support. Please read "
                                 "the Qt SQL driver documentation for information how "
                                 "to build it.\n"
                                 "-- Current error --\n"
                                 "code: %1\n"
                                 "database: %2\n"
                                 "driver: %3\n")
                              .arg(err.nativeErrorCode(), err.databaseText(), err.driverText());

        QMessageBox::critical(nullptr, tr("Cannot open database"), errMsg, QMessageBox::Cancel);
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
    QSqlQuery query = _query();
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
    const QString &sql = "select * from path where parent_id = " + QString::number(parentPathId);
    QSqlQuery query = _query(sql);
    while (query.next()) {
        Path path;
        fillPath(path, query);
        ret.append(path);
    }
    return ret;
}
class Path_qml : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)
    Q_PROPERTY(int pathId READ pathId WRITE setPathId)
public:
    QString name() const { return m_name; }
    void setName(const QString& name) {
        m_name = name;
    }
    QString icon() const { return m_icon; }
    void setIcon(const QString& icon) {
        m_icon = icon;
    }
    int pathId() const { return m_pathId; }
    void setPathId(int pathId) {
        m_pathId = pathId;
    }
private:
    QString m_name;
    QString m_icon;
    int m_pathId;
};
Q_DECLARE_METATYPE(Path_qml*)
class Note_qml : public QObject {
Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)
    Q_PROPERTY(int pathId READ pathId WRITE setPathId)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString basePath READ basePath WRITE setBasePath)
public:
    QString name() const { return m_name; }

    void setName(const QString &name) {
        m_name = name;
    }

    QString icon() const { return m_icon; }

    void setIcon(const QString &icon) {
        m_icon = icon;
    }

    int pathId() const { return m_pathId; }

    void setPathId(int pathId) {
        m_pathId = pathId;
    }

    QString path() const { return m_path; }

    void setPath(const QString &path) {
        m_path = path;
    }

    QString basePath() const { return m_basePath; }

    void setBasePath(const QString &basePath) {
        m_basePath = basePath;
    }

private:
    QString m_name;
    QString m_icon;
    int m_pathId;
    QString m_path;
    QString m_basePath;
};
Q_DECLARE_METATYPE(Note_qml*)
QVariantList DbManager::getPathList_qml(int parentPathId)
{
    auto list = getPathList(parentPathId);
    QVariantList ret;
    for(const auto& it: list) {
        Path_qml* p = new Path_qml();
        p->setName(it.name());
        p->setIcon(Constant::folderImagePath);
        p->setPathId(it.id());
        ret.push_back(QVariant::fromValue(p));
    }
    return ret;
}

QList<Note> DbManager::getNoteList(int pathId) {
    QList<Note> ret;
    const QString &sql = "select * from note where path = " + QString::number(pathId);
    QSqlQuery query = _query(sql);
    while (query.next()) {
        Note note;
        fillNote(note, query);
        ret.append(note);
    }
    return ret;
}

QVariantList DbManager::getNoteList_qml(int pathId) {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    auto notesPath = docPath + "/MyNotes/workshop/";
    auto list = getNoteList(pathId);
    QVariantList ret;
    for (const auto &it: list) {
        Note_qml *p = new Note_qml();
        p->setName(it.title());
        p->setIcon(Constant::noteImagePath);
        p->setPathId(it.pathId());
        p->setPath(notesPath + it.strId() + "/index.md");
        p->setBasePath(notesPath + it.strId() + '/');
        ret.push_back(QVariant::fromValue(p));
    }
    return ret;
}

DbManager::~DbManager() {
    db.close();
}

bool DbManager::addNewNote(Note &note) {
    QSqlQuery query = _query();
    query.prepare("insert into note (str_id, title, path, security) values (:str_id, :title, :path, :security)");
    query.bindValue(":str_id", note.strId());
    query.bindValue(":title", note.title());
    query.bindValue(":path", note.pathId());
    query.bindValue(":security", note.m_security);
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
    // -1表示不存在
    note.m_id = -1;
    const QString &sql = "select * from note where id = " + QString::number(id);
    QSqlQuery query = _query(sql);
    while (query.next()) {
        fillNote(note, query);
    }
    return note;
}

Note DbManager::getNote(const QString &strId)
{
    Note note;
    // -1表示不存在
    note.m_id = -1;
    const QString &sql = "select * from note where str_id = '" + strId + "'";
    qDebug() << "exec sql:" << sql;
    QSqlQuery query = _query(sql);
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
    note.m_security = query.value("security").toInt();
    note.m_createTime = query.value("create_time").toInt();
    note.m_updateTime = query.value("update_time").toInt();
}

bool DbManager::addNewPath(Path &path) {
    QSqlQuery query = _query();
    query.prepare("insert into path (name, parent_id, security) values (:name, :parent_id, :security)");
    query.bindValue(":name", path.name());
    query.bindValue(":parent_id", path.parentId());
    query.bindValue(":security", path.m_security);
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
    const QString &sql = "select * from path where id = " + QString::number(id);
    QSqlQuery query = _query(sql);
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
    path.m_security = query.value("security").toInt();
    path.m_createTime = query.value("create_time").toInt();
    path.m_updateTime = query.value("update_time").toInt();
}

bool DbManager::isPathExist(QString name, int parentId) {
    const QString sql = "select * from path where parent_id = " + QString::number(parentId) + " and name = " + name;
    QSqlQuery query = _query(sql);
    while (query.next()) {
        return true;
    }
    return false;
}

bool DbManager::removeNote(int id) {
    QSqlQuery query = _query();
    query.prepare("update note set trashed = 1 where id = :id");
    query.bindValue(":id", id);
    auto ret = execSql(query);
    return ret;
}

bool DbManager::removePath(int id) {
    QSqlQuery query = _query();
    query.prepare("update path set trashed = 1 where id = :id");
    query.bindValue(":id", id);
    auto ret = execSql(query);
    return ret;
}

bool DbManager::execSql(QSqlQuery &query, bool batch) {
    bool ret;
    if (batch) {
        ret = query.execBatch();
    } else {
        ret = query.exec();
    }
    if (!ret) {
        qDebug() << "exec sql fail: " << query.lastQuery();
        qDebug() << query.lastError().text();
    }
    return ret;
}

bool DbManager::updateIndex(QStringList wordList, int id) {
    db.transaction();
    QSqlQuery query = _query();

    query.prepare("delete from note_word where id=:id");
    query.bindValue(":id", id);
    auto ret = execSql(query);
    if (!ret) {
        qDebug() << "rollback";
        db.rollback();
        return false;
    }
    query.prepare("insert into note_word (word, note) values (:word, :note)");
    query.bindValue(":word", wordList);
    QVariantList idList;
    for(int i=0;i<wordList.size();i++) {
        idList << id;
    }
    query.bindValue(":note", idList);
    ret = execSql(query, true);
    if (!ret) {
        qDebug() << "rollback";
        db.rollback();
        return false;
    }
    db.commit();
    return true;
}

QList<Note> DbManager::getNoteList(QStringList words) {
    QList<Note> ret;
    if (words.empty()) return ret;
    QString sql = QString(R"(
select * from note where id in (
    select distinct note
    from note_word
    where word in ('%1')
)
)").arg(words.join("', '"));
    QSqlQuery query = _query(sql);
    while (query.next()) {
        Note note;
        fillNote(note, query);
        ret.push_back(note);
    }
    return ret;
}

QSqlQuery DbManager::_query() {
    return QSqlQuery(db);
}

QSqlQuery DbManager::_query(QString sql) {
    return QSqlQuery(sql, db);
}

QList<Note> DbManager::getAllNotes() {
    QList<Note> ret;
    const QString &sql = "select * from note";
    QSqlQuery query = _query(sql);
    while (query.next()) {
        Note note;
        fillNote(note, query);
        ret.append(note);
    }
    return ret;
}
#include "DbManager.moc"

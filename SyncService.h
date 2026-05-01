#ifndef MYNOTES_SYNCSERVICE_H
#define MYNOTES_SYNCSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include "DbModel.h"

class ElasticSearchRestApi;
class HtmlExporter;
class Settings;
class DbManager;
struct ServerNoteInfo;

class SyncService : public QObject {
    Q_OBJECT
public:
    explicit SyncService(const QString& notesPath, ElasticSearchRestApi* esApi, HtmlExporter* htmlExporter, DbManager* dbManager, QObject *parent = nullptr);

    void syncAll();
    void syncAllWatching();
    void syncWatchingFolder(const QString& path);
    void syncWatchingFile(const QString& path);
    void syncWorkshopFile(const Note& note);
    void uploadNoteAttachment(const Note& note);
    void uploadFile(const QString& noteStrId, const QString& filePath);
    void uploadNote(const ServerNoteInfo& info);
    void updateProfile();
    void showSyncResult(const QString& msg);

private:
    void doUpdateProfile();
    void traversalFileTree(const QString& path, QStringList& pathList);
    static QByteArray fileChecksum(const QString& fileName, QCryptographicHash::Algorithm hashAlgorithm);
    QString noteRealPath(const Note& note) const;
    inline QString workshopPath() const { return m_notesPath + "/workshop/"; }

    ElasticSearchRestApi* m_esApi;
    HtmlExporter* m_htmlExporter;
    Settings* m_settings;
    DbManager* m_dbManager;
    QString m_notesPath;
};

#endif

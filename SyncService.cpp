#include "SyncService.h"
#include "ElasticSearchRestApi.h"
#include "HtmlExporter.h"
#include "DbManager.h"
#include "Settings.h"
#include "Http.h"
#include "Utils.h"
#include "WatchingFileHtmlVisitor.h"
#include "QtMarkdownParser"
#include <QtWordReader>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

SyncService::SyncService(const QString& notesPath, ElasticSearchRestApi* esApi, HtmlExporter* htmlExporter, DbManager* dbManager, QObject *parent)
    : QObject(parent), m_notesPath(notesPath), m_esApi(esApi), m_htmlExporter(htmlExporter), m_dbManager(dbManager)
{
    m_settings = Settings::instance();
}

QByteArray SyncService::fileChecksum(const QString &fileName,
                                     QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

QString SyncService::noteRealPath(const Note& note) const {
    return workshopPath() + note.strId() + "/index.md";
}

void SyncService::syncAll() {
    auto notes = m_dbManager->getAllNotes();
    for(const auto& note: notes) {
        if (note.trashed()) continue;
        QString owner = m_settings->usernameEn;
        auto html = m_htmlExporter->generateHtml(note);
        m_esApi->putNote(owner, html, note);
        uploadNoteAttachment(note);
    }
}

void SyncService::uploadNoteAttachment(const Note &note) {
    auto http = Http::instance();
    QString owner = m_settings->usernameEn;
    QString noteId = note.strId();
    QString serverIp = m_settings->serverIp;
    auto uploadFile = [http, owner, noteId, serverIp](QFile file) {
        QString checksum = fileChecksum(file.fileName(), QCryptographicHash::Sha512).toHex();
        qDebug() << "checksum:" << checksum;
        QString _url = QString("http://%1:9201/upload").arg(serverIp);
        const QString &filename = QFileInfo(file).fileName();
        QString staticFileServerBaseUrl = QString("http://%1").arg(serverIp);
        QString serverFilePath = QString("/%1/%2/%3.checksum.txt").arg(owner).arg(noteId).arg(filename);
        auto serverFileChecksum = http->get(QString("%1%2").arg(staticFileServerBaseUrl).arg(serverFilePath));
        auto realServerFileChecksum = serverFileChecksum.left(checksum.size());
        qDebug() << "serverFileChecksum:" << realServerFileChecksum;
        if (realServerFileChecksum == checksum) {
            qDebug () << "ignore file: " << filename;
            return ;
        } else {
            QString checksumUploadUrl = QString("%1?owner=%2&filename=%3.checksum.txt&note_id=%4").arg(_url).arg(owner).arg(filename).arg(noteId);
            QString magic = "\nasldjlaskfdjlasdjfklsajdfkljasldflalsfdkajsf";
            for (int i=0;i<20;i++)
                checksum += magic;
            auto res = http->uploadFile(checksumUploadUrl, checksum.toUtf8());
            qDebug() << "res:" << res;
        }
        QString uploadFileUrl = QString("%1?owner=%2&filename=%3&note_id=%4").arg(_url).arg(owner).arg(filename).arg(noteId);
        auto res = http->uploadFile(uploadFileUrl, file);
        qDebug() << "res:" << res;
    };
    auto notePath = noteRealPath(note);
    auto dir = QFileInfo(notePath).dir();
    auto infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for(const auto& info: infoList) {
        if (info.fileName() == "index.md") continue;
        uploadFile(QFile(info.absoluteFilePath()));
    }
    auto html = m_htmlExporter->generateHtml(note);
    QString uploadHtmlUrl = QString("http://%1:9201/upload?owner=%2&filename=index.html&note_id=%3").arg(serverIp).arg(owner).arg(noteId);
    http->uploadFile(uploadHtmlUrl, html.toUtf8());
}

void SyncService::uploadNote(const ServerNoteInfo& info) {
    m_esApi->putNote(info);
    QFile cssFile(":css/css518.css");
    cssFile.open(QIODevice::ReadOnly);
    QString css = cssFile.readAll();
    cssFile.close();
    QString mdCssPath = "github-markdown.css";
    auto html = R"(<!DOCTYPE html><html><head>
<meta charset="utf-8">
<meta name='viewport' content='width=device-width initial-scale=1'>
<title>)"
           +
           info.title
           +
           R"(</title>
<link href='https://fonts.loli.net/css?family=Open+Sans:400italic,700italic,700,400&subset=latin,latin-ext' rel='stylesheet' type='text/css' />

<style type='text/css'>)"
           +
           css
           +
           R"("</style>
</head>
<body class='typora-export'>
<div id='write'  class=''>)"
           +
           info.noteHtml
           +
           R"(</div></body></html>)";
    auto http = Http::instance();
    QString serverIp = m_settings->serverIp;
    QString owner = m_settings->usernameEn;
    QString baseUrl = QString("http://%1:9201/upload").arg(serverIp);
    QString uploadFileUrl = QString("%1?owner=%2&filename=index.html&note_id=%4")
            .arg(baseUrl).arg(owner).arg(info.strId);
    auto res = http->uploadFile(uploadFileUrl, html.toUtf8());
    qDebug() << "res:" << res;
}

void SyncService::syncWatchingFile(const QString& path) {
    qInfo() << "sync watching file:" << path;
    auto syncNote = [this](const QString& path) {
        auto title = QFileInfo(path).baseName();
        QFile mdFile(path);
        bool ok = mdFile.open(QIODevice::ReadOnly);
        if (!ok) {
            qWarning() << "open fail:" << path;
        }
        Document doc(mdFile.readAll());
        mdFile.close();
        WatchingFileHtmlVisitor visitor(path);
        doc.accept(&visitor);
        auto html = visitor.html();
        auto pathList = visitor.pathList();
        QString owner = m_settings->usernameEn;
        ServerNoteInfo info;
        info.title = title;
        info.owner = owner;
        info.noteHtml = html;
        info.strId = Utils::md5(path);
        uploadNote(info);
        for(const auto& attachmentFilePath: pathList) {
            qDebug() << "upload attachment:" << attachmentFilePath;
            uploadFile(info.strId, attachmentFilePath);
        }
    };
    if (path.endsWith(".md")) {
        syncNote(path);
    }
    else if (path.endsWith(".txt")) {

    }
    else if (path.endsWith(".docx")) {
        qDebug() << "sync" << path;
        auto title = QFileInfo(path).baseName();
        WordReader reader(path);
        auto wordContent = reader.readAll();
        QString html;
        html += QString(R"(<a href="%1">点击下载原文件</a>)").arg(QFileInfo(path).fileName());
        html += "<h1>" + title + "</h1>\n";
        for(auto p: wordContent.split("\n")) {
            html += "<p>" + p + "</p>";
        }
        QString owner = m_settings->usernameEn;
        ServerNoteInfo info;
        info.title = title;
        info.owner = owner;
        info.noteHtml = html;
        info.strId = Utils::md5(path);
        uploadNote(info);
        qDebug() << "upload word(.doxc):" << path;
        uploadFile(info.strId, path);
    }
}

void SyncService::syncWatchingFolder(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << dir << "not exist.";
        return;
    }
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < info_list.count(); i++) {
        auto info = info_list[i];
        if (info.isFile() && !info.fileName().endsWith(".md")) continue;
        const QString &filePath = info.absoluteFilePath();
        if (info.isDir()) {
            syncWatchingFolder(filePath);
        } else {
            syncWatchingFile(filePath);
        }
    }
}

void SyncService::syncWorkshopFile(const Note& note) {
    qInfo() << "sync workshop file:" << note.strId();
    auto path = noteRealPath(note);
    QFile mdFile(path);
    mdFile.open(QIODevice::ReadOnly);
    Document doc(mdFile.readAll());
    mdFile.close();
    WatchingFileHtmlVisitor visitor(path);
    doc.accept(&visitor);
    auto html = visitor.html();
    auto pathList = visitor.pathList();
    QString owner = m_settings->usernameEn;
    ServerNoteInfo info;
    info.title = note.title();
    info.owner = owner;
    info.noteHtml = html;
    info.strId = note.strId();
    uploadNote(info);
    for(const auto& attachmentFilePath: pathList) {
        qDebug() << "upload attachment:" << attachmentFilePath;
        uploadFile(info.strId, attachmentFilePath);
    }
}

void SyncService::syncAllWatching() {
    QStringList watchingDirs = m_settings->watchingFolders;
    for(const auto& dir: watchingDirs) {
        syncWatchingFolder(dir);
    }
    showSyncResult(tr("Sync All Watching Success"));
}

void SyncService::showSyncResult(const QString& msg) {
    QMessageBox::information(nullptr,
                             tr("Sync Result"),
                             msg
    );
}

void SyncService::updateProfile() {
    if (m_settings->modeOffline) {
        qDebug() << "offline mode. ignore updateProfile";
    } else {
        doUpdateProfile();
    }
}

void SyncService::doUpdateProfile() {
    qDebug() << "update profile";
    QStringList watchingFolders = m_settings->watchingFolders;
    QStringList pathList;
    for(const QString& folderPath: watchingFolders) {
        traversalFileTree(folderPath, pathList);
    }
    QString owner = m_settings->usernameEn;
    QString html = R"(
<!DOCTYPE html><html><head>
<meta charset="utf-8">
<meta name='viewport' content='width=device-width initial-scale=1'>
<title>"+
                   owner +
                   R"( Home</title>
<body>
)";
    html.append(QString("<h1>%1的主页</h1>").arg(m_settings->usernameZh));
    html.append(QString("共%1个笔记").arg(pathList.size()));
    html.append("<ul>");
    for(const QString& path: pathList) {
        QFileInfo fileInfo(path);
        QString raw = R"(<li><a href="/%1" target="_blank">%2</a></li>)";
        QString url = QString("%1/%2/index.html")
                .arg(owner).arg(Utils::md5(path));
        html.append(raw.arg(url).arg(fileInfo.baseName()));
    }
    html.append("</ul>");
    html += R"(
</body>
</html>
)";
    QString serverIp = m_settings->serverIp;
    QString url = QString("http://%1:9201/upload_profile?owner=%2&filename=index.html")
            .arg(serverIp).arg(owner);
    Http::instance()->uploadFile(url, html.toUtf8());
}

void SyncService::uploadFile(const QString& noteStrId, const QString& path) {
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        qWarning() << "file not exist." << path;
    }
    auto http = Http::instance();
    QString serverIp = m_settings->serverIp;
    QString owner = m_settings->usernameEn;
    QString checksum = fileChecksum(path, QCryptographicHash::Sha512).toHex();
    qDebug() << "checksum:" << checksum;
    QString _url = QString("http://%1:9201/upload").arg(serverIp);
    const QString &filename = fileInfo.fileName();
    QString staticFileServerBaseUrl = QString("http://%1").arg(serverIp);
    QString serverFilePath = QString("/%1/%2/%3.checksum.txt")
            .arg(owner).arg(noteStrId).arg(filename);
    auto serverFileChecksum = http->get(QString("%1%2")
            .arg(staticFileServerBaseUrl).arg(serverFilePath));
    auto realServerFileChecksum = serverFileChecksum.left(checksum.size());
    qDebug() << "serverFileChecksum:" << realServerFileChecksum;
    if (realServerFileChecksum == checksum) {
        qDebug () << "ignore file: " << filename;
        return ;
    } else {
        QString checksumUploadUrl = QString("%1?owner=%2&filename=%3.checksum.txt&note_id=%4")
                .arg(_url).arg(owner).arg(filename).arg(noteStrId);
        QString magic = "\nasldjlaskfdjlasdjfklsajdfkljasldflalsfdkajsf";
        for (int i=0;i<20;i++)
            checksum += magic;
        auto res = http->uploadFile(checksumUploadUrl, checksum.toUtf8());
        qDebug() << "res:" << res;
    }
    QString uploadFileUrl = QString("%1?owner=%2&filename=%3&note_id=%4")
            .arg(_url).arg(owner).arg(filename).arg(noteStrId);
    QFile _file(path);
    auto res = http->uploadFile(uploadFileUrl, _file);
    qDebug() << "res:" << res;
}

void SyncService::traversalFileTree(const QString& path, QStringList& pathList) {
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << dir << "not exist.";
        return;
    }
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < info_list.count(); i++) {
        auto info = info_list[i];
        const QString &filePath = info.absoluteFilePath();
        if (info.isDir()) {
            traversalFileTree(filePath, pathList);
        } else {
            for(const auto& suffix: Utils::syncSuffix()) {
                if (filePath.endsWith(suffix)) {
                    pathList.append(filePath);
                }
            }
        }
    }
}

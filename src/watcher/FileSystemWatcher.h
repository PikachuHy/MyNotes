//
// Created by PikachuHy on 2021/4/1.
//

#ifndef MYNOTES_FILESYSTEMWATCHER_H
#define MYNOTES_FILESYSTEMWATCHER_H
#include <QFileSystemWatcher>
#include <QMap>
#include <QStringList>

class FileSystemWatcher: public QObject {
    Q_OBJECT
public:
    static FileSystemWatcher *instance();
    bool addPath(const QString& path);
    QStringList files();
Q_SIGNALS:
    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);
    void renameFile(const QString& oldFilePath, const QString& newFilePath);
    void renameFolder(const QString& oldFolderPath, const QString& newFolderPath);
    void newFile(const QString& newFilePath);
    void newFolder(const QString& newFolderPath);
    void deleteFile(const QString& deletedFilePath);
    void deleteFolder(const QString& deletedFolderPath);
private:
    FileSystemWatcher();
    ~FileSystemWatcher();
    QFileSystemWatcher* m_fileSystemWatcher;
    QMap<QString, QStringList> m_path2childrenMap;
};


#endif //MYNOTES_FILESYSTEMWATCHER_H

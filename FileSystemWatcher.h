//
// Created by PikachuHy on 2021/4/1.
//

#ifndef MYNOTES_FILESYSTEMWATCHER_H
#define MYNOTES_FILESYSTEMWATCHER_H
#include <QFileSystemWatcher>

class FileSystemWatcher: public QObject {
    Q_OBJECT
public:
    static FileSystemWatcher *instance();
Q_SIGNALS:
    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);
private:
    FileSystemWatcher();
    ~FileSystemWatcher();
    QFileSystemWatcher* m_fileSystemWatcher;
};


#endif //MYNOTES_FILESYSTEMWATCHER_H

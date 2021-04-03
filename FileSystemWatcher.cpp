//
// Created by PikachuHy on 2021/4/1.
//

#include "FileSystemWatcher.h"
#include <QFileInfo>
#include <QDir>
#include <QSet>
#include <QDebug>
FileSystemWatcher::FileSystemWatcher() {
    m_fileSystemWatcher = new QFileSystemWatcher();
    connect(m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString &changedPath){
        qDebug() << "dir changed:" << changedPath;
        // 查文件的变化
        // 参考：https://blog.csdn.net/liang19890820/article/details/51849252
        QStringList children = m_path2childrenMap[changedPath];
        const QDir dir(changedPath);
        auto newChildren = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
#if (QT_VERSION <= QT_VERSION_CHECK(5,15,0))
        QSet<QString> newChildrenSet = QSet<QString>::fromList(newChildren);
        QSet<QString> childrenSet = QSet<QString>::fromList(children);
#else
        QSet<QString> newChildrenSet(newChildren.begin(), newChildren.end());
        QSet<QString> childrenSet(children.begin(), children.end());
#endif
        auto newNameSet = newChildrenSet - childrenSet;
        auto newNames = newNameSet.values();
        auto deletedNameSet = childrenSet - newChildrenSet;
        auto deletedNames = deletedNameSet.values();
        m_path2childrenMap[changedPath] = newChildren;
        qDebug() << "new path:" << newNames;
        qDebug() << "deleted path:" << deletedNames;
        if (!newNames.isEmpty() && !deletedNames.isEmpty()) {
            // 重命名的情况
            if (newNames.size() == 1 && deletedNames.size() == 1) {
                auto oldPath = changedPath + "/" + deletedNames.first();
                auto newPath = changedPath + "/" + newNames.first();
                qDebug() << oldPath << "->" << newPath;
                if (QFileInfo(newPath).isDir()) {
                    emit renameFolder(oldPath, newPath);
                } else {
                    emit renameFile(oldPath, newPath);
                }
            } else {
                // TODO: 其他的处理
                qWarning() << "unknown case. "
                           << "newFiles:" << newNames
                           << "deletedFiles:" << deletedNames;
            }
        } else {
            // 新建文件/文件夹
            if (!newNames.isEmpty()) {
                for(const auto& name: newNames) {
                    QString path = changedPath + '/' + name;
                    if (QFileInfo(path).isDir()) {
                        emit newFolder(path);
                    } else {
                        emit newFile(path);
                    }
                }
            }
            // 删除文件，文件夹
            if (!deletedNames.isEmpty()) {
                for(const auto& name: deletedNames) {
                    QString path = changedPath + '/' + name;
                    if (QFileInfo(path).isDir()) {
                        emit deleteFolder(path);
                    } else {
                        emit deleteFile(path);
                    }
                }
            }
        }
        emit directoryChanged(changedPath);
    });
    connect(m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, [this](const QString &path){
        emit fileChanged(path);
    });
}

FileSystemWatcher::~FileSystemWatcher() {
    delete m_fileSystemWatcher;
}

FileSystemWatcher *FileSystemWatcher::instance() {
    static FileSystemWatcher fileSystemWatcher;
    return &fileSystemWatcher;
}

bool FileSystemWatcher::addPath(const QString &path) {
    qDebug() << "watch" << path;
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        const QDir dir(path);
        m_path2childrenMap[path] = dir.entryList(
                QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst
                );
        qDebug() << path << ":" << m_path2childrenMap[path];
    }
    return m_fileSystemWatcher->addPath(path);
}

QStringList FileSystemWatcher::files() {
    return m_fileSystemWatcher->files();
}

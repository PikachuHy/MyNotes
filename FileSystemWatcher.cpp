//
// Created by PikachuHy on 2021/4/1.
//

#include "FileSystemWatcher.h"

FileSystemWatcher::FileSystemWatcher() {
    m_fileSystemWatcher = new QFileSystemWatcher();
    connect(m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString &path){
        emit directoryChanged(path);
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

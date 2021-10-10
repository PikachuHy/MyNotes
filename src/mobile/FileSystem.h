//
// Created by pikachu on 2021/10/5.
//

#ifndef MYNOTES_FILESYSTEM_H
#define MYNOTES_FILESYSTEM_H
#include <QObject>
#include <QStringList>
class FileSystem : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE
    QStringList listDir(QString path);
    Q_INVOKABLE
    bool isDir(QString path);
    Q_INVOKABLE
    bool isFile(QString path);
    Q_INVOKABLE
    QString defaultPath();
};
#endif //MYNOTES_FILESYSTEM_H

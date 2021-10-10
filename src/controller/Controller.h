//
// Created by pikachu on 2021/6/14.
//

#ifndef MYNOTES_CONTROLLER_H
#define MYNOTES_CONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include <QRect>
class Controller: public QObject {
Q_OBJECT
public:
    Controller(QObject *parent = nullptr);

    Q_INVOKABLE QString lastOpenedNote();
    Q_INVOKABLE void setLastOpenedNote(QString path);
    Q_INVOKABLE void openUrl(QString url);
    Q_INVOKABLE QRect lastWindowRect();
    Q_INVOKABLE void setLastWindowRect(QRect rect);
    Q_INVOKABLE void watchNote(const QString& path);
    Q_INVOKABLE QString notePathFromModelIndex(QModelIndex index);
    Q_INVOKABLE bool isNoteItem(QModelIndex index);

    Q_INVOKABLE QString noteDataPath();
    Q_INVOKABLE void setNoteDataPath(QString path);
    Q_INVOKABLE QString configStorePath();

    Q_INVOKABLE bool hasWriteExternalStoragePermission();
    Q_INVOKABLE bool hasManageExternalStorage();
signals:
    void noteChanged(QString path);
private:
};


#endif //MYNOTES_CONTROLLER_H

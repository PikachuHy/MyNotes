//
// Created by pikachu on 2021/6/24.
//

#ifndef MYNOTES_NOTECONTROLLER_H
#define MYNOTES_NOTECONTROLLER_H
#include <QObject>
class NoteController: public QObject {
Q_OBJECT
public:
    explicit NoteController(QObject *parent = nullptr);

    Q_INVOKABLE QModelIndex createNewNote(QModelIndex index, QString noteName);
    Q_INVOKABLE QModelIndex createNewFolder(QModelIndex index, QString folderName);
    Q_INVOKABLE void trashNote(QModelIndex index);
    Q_INVOKABLE void trashFolder(QModelIndex index);
    Q_INVOKABLE bool isNote(QModelIndex index);
    Q_INVOKABLE QString getNoteFullPath(QModelIndex index);
    Q_INVOKABLE int getPathId(QModelIndex index);
private:
};


#endif //MYNOTES_NOTECONTROLLER_H

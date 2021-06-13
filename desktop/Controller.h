//
// Created by pikachu on 2021/6/14.
//

#ifndef MYNOTES_CONTROLLER_H
#define MYNOTES_CONTROLLER_H

#include <QObject>
#include <QModelIndex>
class Controller: public QObject {
Q_OBJECT
public:
    Q_INVOKABLE bool isNote(QModelIndex index);
    Q_INVOKABLE QString getNoteFullPath(QModelIndex index);
private:
};


#endif //MYNOTES_CONTROLLER_H

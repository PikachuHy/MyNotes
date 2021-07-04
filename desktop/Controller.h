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
    Q_INVOKABLE QString lastOpenedNote();
    Q_INVOKABLE void setLastOpenedNote(QString path);
    Q_INVOKABLE void openUrl(QString url);
    Q_INVOKABLE QRect lastWindowRect();
private:
};


#endif //MYNOTES_CONTROLLER_H

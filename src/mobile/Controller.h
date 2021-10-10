//
// Created by pikachu on 2021/10/5.
//

#ifndef MYNOTES_CONTROLLER_H
#define MYNOTES_CONTROLLER_H
#include <QObject>
class Controller : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE
    QString noteDataPath();
    Q_INVOKABLE
    void setNoteDataPath(QString path);
    Q_INVOKABLE
    QString configStorePath();
};
#endif //MYNOTES_CONTROLLER_H

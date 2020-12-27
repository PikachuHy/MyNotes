//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_LISTMODEL_H
#define MYNOTES_LISTMODEL_H

#include <QAbstractListModel>
#include "DbModel.h"
class ListModel : public QAbstractListModel{
Q_OBJECT
public:
    explicit ListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    void reset(QList<Note> noteList);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QList<Note> m_noteList;
};


#endif //MYNOTES_LISTMODEL_H

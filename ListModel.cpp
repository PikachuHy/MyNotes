//
// Created by PikachuHy on 2020/12/27.
//

#include "ListModel.h"

#include <utility>

void ListModel::reset(QList<Note> noteList) {
    beginRemoveRows(index(0, 0), 0, m_noteList.size());
    endRemoveRows();
    beginInsertRows(index(0, 0), 0, noteList.size());
    m_noteList = std::move(noteList);
    endInsertRows();
}

int ListModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        qDebug() << "parent index is invalid";
        return 0;
    }
    qDebug() << "row count: " << m_noteList.size();
    return m_noteList.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        int row = index.row();
        if (index.column() == 0 && row >= 0 && row < m_noteList.size()) {
            return m_noteList[row].title();
        }
    }
    return QVariant();
}

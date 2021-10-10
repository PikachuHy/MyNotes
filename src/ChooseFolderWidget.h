//
// Created by PikachuHy on 2021/3/16.
//

#ifndef MYNOTES_CHOOSEFOLDERWIDGET_H
#define MYNOTES_CHOOSEFOLDERWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include "DbManager.h"
class ChooseFolderWidget: public QTreeWidget {
Q_OBJECT
public:
    ChooseFolderWidget(DbManager* dbManager, QWidget* parent = nullptr);
private:
    DbManager* m_dbManager;
};


#endif //MYNOTES_CHOOSEFOLDERWIDGET_H

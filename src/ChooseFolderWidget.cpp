//
// Created by PikachuHy on 2021/3/16.
//

#include "ChooseFolderWidget.h"

ChooseFolderWidget::ChooseFolderWidget(DbManager *dbManager, QWidget *parent):
    m_dbManager(dbManager), QTreeWidget(parent)
{
    setColumnCount(1);
    QList<QTreeWidgetItem *> items;
//    for (int i = 0; i < 10; ++i)
//        items.append();
//    treeWidget->insertTopLevelItems(0, items);
//    auto pathList = m_dbManager->getPathList(0);
//    for(const auto& path: pathList) {
//        if(path.trashed()) continue;
//        auto item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), path.name());
//
//    }
}

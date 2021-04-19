//
// Created by pikachu on 2021/4/19.
//

#include "TabBar.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
TabBar::TabBar(QWidget *parent) : QTabBar(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTabBar::customContextMenuRequested, [this](const QPoint &pos){
        qDebug() << pos;
        QMenu menu;
        // TODO: chrome-like menu
        menu.exec(this->mapToGlobal(pos));
    });
}

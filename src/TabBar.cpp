//
// Created by pikachu on 2021/4/19.
//

#include "TabBar.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
TabBar::TabBar(QWidget *parent) : QTabBar(parent) {
    setContextMenuPolicy(Qt::CustomContextMenu);

}

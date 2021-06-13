//
// Created by pikachu on 6/13/2021.
//

#include "MainWindow.h"
#include "Widget.h"
#include <QMenuBar>
#include <QAction>
#include <QDebug>

MainWindow::MainWindow() {
    setCentralWidget(new Widget());
    auto bar = this->menuBar();
    if (bar == nullptr) {
        bar = new QMenuBar();
        setMenuBar(bar);
    }
    auto menu = new QMenu();
    bar->addMenu(menu);
    menu->addAction("", this, [this]() {

    }, QKeySequence(Qt::MetaModifier | Qt::Key_Tab));
}

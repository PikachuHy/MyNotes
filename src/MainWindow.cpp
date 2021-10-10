//
// Created by pikachu on 6/13/2021.
//

#include "MainWindow.h"
#include "Widget.h"
#include "Settings.h"
#include <QMenuBar>
#include <QAction>
#include <QDebug>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow() {
    auto w = new Widget();
    setCentralWidget(w);
    auto bar = this->menuBar();
    if (bar == nullptr) {
        bar = new QMenuBar();
        setMenuBar(bar);
    }
    auto menu = new QMenu();
    bar->addMenu(menu);
    menu->addAction("", this, [w]() {
        w->showNextTab();
    }, QKeySequence(Qt::MetaModifier | Qt::Key_Tab));


    auto screenSize = QApplication::primaryScreen()->size();
    QRect winGeometry = Settings::instance()->mainWindowGeometry;
    if (!winGeometry.isValid()) {
        winGeometry = QRect(
                (screenSize.width()-1500) / 2,
                (screenSize.height()-800) / 2,
                1500, 800
        );
        Settings::instance()->mainWindowGeometry = winGeometry;
    }
    // 兼容分辨率比较小的电脑
    int finalWidth = screenSize.width() < winGeometry.width() ? screenSize.width() : winGeometry.width();
    int finalHeight = screenSize.height() < winGeometry.height() ? screenSize.height() : winGeometry.height();

    winGeometry = QRect(
            (screenSize.width()-finalWidth) / 2,
            (screenSize.height()-finalHeight) / 2,
            finalWidth, finalHeight
    );
    Settings::instance()->mainWindowGeometry = winGeometry;
    qDebug() << "load window geometry" << winGeometry;
    setGeometry(winGeometry);
}

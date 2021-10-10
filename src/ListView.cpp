//
// Created by PikachuHy on 2020/12/27.
//

#include "ListView.h"
#include "Constant.h"
#include <QKeyEvent>
#include <QPainter>

ListView::ListView(QWidget *parent) : QListView(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setMinimumWidth(Constant::searchDialogWidth);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ListView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
        return;
    }
    QAbstractItemView::keyPressEvent(event);
}

void ListView::paintEvent(QPaintEvent *e) {
    QListView::paintEvent(e);
    if (model() && model()->rowCount() > 0) return;
    QPainter painter(viewport());
    painter.drawText(rect(), Qt::AlignCenter, tr("No Results"));
}

//
// Created by PikachuHy on 2020/12/27.
//

#include "ListView.h"
#include "Constant.h"
#include <QKeyEvent>

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

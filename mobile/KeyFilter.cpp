//
// Created by pikachu on 5/23/2021.
//

#include "KeyFilter.h"
#include <QEvent>
#include <QKeyEvent>

void KeyFilter::setFilter(QObject *o) {
    o->installEventFilter(this);
}

bool KeyFilter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Back) {
            emit keyBackPress();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

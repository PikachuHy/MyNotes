//
// Created by pikachu on 2021/6/24.
//

#include "Clipboard.h"
#include <QGuiApplication>
#include <QClipboard>
void Clipboard::copyText(QString text) {
    QGuiApplication::clipboard()->setText(text);
}

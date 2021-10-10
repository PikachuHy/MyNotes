//
// Created by pikachu on 2021/4/12.
//

#include "WebEngineView.h"

QWebEngineView *WebEngineView::createWindow(QWebEnginePage::WebWindowType type) {
    Q_UNUSED(type);
    return this;
}

//
// Created by pikachu on 2021/6/14.
//

#include "Controller.h"
#include "TreeItem.h"
#include "Settings.h"
#include "TreeItem.h"
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>

QString Controller::lastOpenedNote() {
    return Settings::instance()->lastOpenNotePath;
}

void Controller::setLastOpenedNote(QString path) {
    Settings::instance()->lastOpenNotePath = path;
}

void Controller::openUrl(QString url) {
    QDesktopServices::openUrl(QUrl(url));
}

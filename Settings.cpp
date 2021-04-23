//
// Created by PikachuHy on 2021/3/25.
//

#include "Settings.h"
#include <QApplication>
const char Settings::KEY_LAST_OPEN_NOTE_PATH[] = "path/last_open_note";
const char Settings::KEY_TYPORA_PATH[] = "path/typora";
const char Settings::KEY_TROJAN_CONFIG_PATH[] = "path/trojan_config";
const char Settings::KEY_SERVER_IP[] = "server/ip";
const char Settings::KEY_USER_ACCOUNT[] = "user/account";
const char Settings::KEY_USER_PASSWORD[] = "user/password";
const char Settings::KEY_USER_NAME_ZH[] = "user/name_zh";
const char Settings::KEY_USER_NAME_EN[] = "user/name_en";
const char Settings::KEY_USER_REMEMBER_PASSWORD[] = "user/remember_password";
const char Settings::KEY_USER_AUTO_LOGIN[] = "user/auto_login";
const char Settings::KEY_USER_SIGNATURE[] = "user/signature";
const char Settings::KEY_MAIN_WINDOW_GEOMETRY[] = "main_window/geometry";
const char Settings::KEY_SYNC_VERSION[] = "sync/version";
const char Settings::KEY_SYNC_WORKSHOP_AUTO[] = "sync/workshop_auto";
const char Settings::KEY_SYNC_WATCHING_AUTO[] = "sync/watching_auto";
const char Settings::KEY_WATCHING_FOLDERS[] = "watching/folders";
const char Settings::KEY_MODE_OFFLINE[] = "mode/offline";
Settings *Settings::instance()
{
    static Settings singleton;
    return &singleton;
}

Settings::Settings(){}

QSettings *Settings::settings() {
    static QSettings ret(QSettings::IniFormat,
              QSettings::UserScope,
              QApplication::organizationName(),
              QApplication::applicationName()
    );
    return &ret;
}

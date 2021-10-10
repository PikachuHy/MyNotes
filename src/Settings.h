//
// Created by PikachuHy on 2021/3/25.
//

#ifndef MYNOTES_SETTINGS_H
#define MYNOTES_SETTINGS_H

#include <QSettings>
#include <QRect>

class Settings {
public:
    static Settings *instance();
    QString configStorePath();

    static const char KEY_LAST_OPEN_NOTE_PATH[];
    static const char KEY_TYPORA_PATH[];
    static const char KEY_TROJAN_CONFIG_PATH[];
    static const char KEY_NOTE_DATA_PATH[];
    static const char KEY_SERVER_IP[];
    static const char KEY_USER_ACCOUNT[];
    static const char KEY_USER_PASSWORD[];
    static const char KEY_USER_NAME_ZH[];
    static const char KEY_USER_NAME_EN[];
    static const char KEY_USER_REMEMBER_PASSWORD[];
    static const char KEY_USER_AUTO_LOGIN[];
    static const char KEY_USER_SIGNATURE[];
    static const char KEY_MAIN_WINDOW_GEOMETRY[];
    static const char KEY_SYNC_VERSION[];
    static const char KEY_SYNC_WORKSHOP_AUTO[];
    static const char KEY_SYNC_WATCHING_AUTO[];
    static const char KEY_WATCHING_FOLDERS[];
    static const char KEY_MODE_OFFLINE[];
    static const char KEY_MODE_RENDER[];
    static const char KEY_MODE_HIDPI[];

    template<const char *key>
    struct QStringRef {
        operator QString() const {
            // 解决中文乱码问题
            return QString::fromUtf8(settings()->value(key).toByteArray());
        }

        QStringRef &operator=(const QString &newValue) {
            settings()->setValue(key, newValue);
            return *this;
        }
    };

    template<const char *key>
    struct IntRef {
        operator int() const {
            return settings()->value(key).toInt();
        }

        IntRef &operator=(const int &newValue) {
            settings()->setValue(key, newValue);
            return *this;
        }
    };

    template<const char *key>
    struct BoolRef {
        operator bool() const {
            return settings()->value(key).toBool();
        }

        BoolRef &operator=(const bool &newValue) {
            settings()->setValue(key, newValue);
            return *this;
        }
    };

    template<const char *key>
    struct QRectRef {
        operator QRect() const {
            return settings()->value(key).toRect();
        }

        QRectRef &operator=(const QRect &newValue) {
            settings()->setValue(key, newValue);
            return *this;
        }
    };
    template<const char *key>
    struct QStringListRef {
        operator QStringList() const {
            return settings()->value(key).toStringList();
        }

        QStringListRef &operator=(const QStringList &newValue) {
            settings()->setValue(key, newValue);
            return *this;
        }
    };

    QStringRef<KEY_LAST_OPEN_NOTE_PATH> lastOpenNotePath;
    QStringRef<KEY_TYPORA_PATH> typoraPath;
    QStringRef<KEY_TROJAN_CONFIG_PATH> trojanConfigPath;
    QStringRef<KEY_NOTE_DATA_PATH> noteDataPath;
    QStringRef<KEY_SERVER_IP> serverIp;
    QStringRef<KEY_USER_ACCOUNT> userAccount;
    QStringRef<KEY_USER_PASSWORD> userPassword;
    QStringRef<KEY_USER_NAME_ZH> usernameZh;
    QStringRef<KEY_USER_NAME_EN> usernameEn;
    BoolRef<KEY_USER_REMEMBER_PASSWORD> userRememberPassword;
    BoolRef<KEY_USER_AUTO_LOGIN> userAutoLogin;
    QStringRef<KEY_USER_SIGNATURE> userSignature;
    QRectRef<KEY_MAIN_WINDOW_GEOMETRY> mainWindowGeometry;
    IntRef<KEY_SYNC_VERSION> syncVersion;
    BoolRef<KEY_SYNC_WORKSHOP_AUTO> syncWorkshopAuto;
    BoolRef<KEY_SYNC_WATCHING_AUTO> syncWatchingAuto;
    QStringListRef<KEY_WATCHING_FOLDERS> watchingFolders;
    BoolRef<KEY_MODE_OFFLINE> modeOffline;
    IntRef<KEY_MODE_RENDER> modeRender;
    BoolRef<KEY_MODE_HIDPI> modeHiDPI;

// Singleton, to be used by any part of the app
private:
    static QSettings *settings();

    Settings();

    Settings(const Settings &other);

    Settings &operator=(const Settings &other);
};


#endif //MYNOTES_SETTINGS_H

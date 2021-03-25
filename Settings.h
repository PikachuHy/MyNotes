//
// Created by PikachuHy on 2021/3/25.
//

#ifndef MYNOTES_SETTINGS_H
#define MYNOTES_SETTINGS_H

#include <QSettings>
class Settings
{
public:
    static Settings *instance();
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    void apply(QVariant setting, std::function<void(QVariant)> handler);

// Singleton, to be used by any part of the app
private:
    Settings();
    Settings(const Settings& other);
    Settings &operator=(const Settings& other);
    QSettings settings;
};


#endif //MYNOTES_SETTINGS_H

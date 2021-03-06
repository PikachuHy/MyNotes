//
// Created by PikachuHy on 2020/12/25.
//

#include "Utils.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <QThread>
#include <QTimer>
#include <QCryptographicHash>
#include <QWidget>

using namespace std;
std::string chMap = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"s;
namespace Utils {
    time_t getTimeStamp() {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now());
        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
        time_t timestamp = tmp.count();
        //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
        return timestamp;
    }
    QString generateId() {
        time_t stamp = getTimeStamp();
        std::string ret;
        while (stamp) {
            ret += chMap[stamp % chMap.size()];
            stamp /= chMap.size();
        }

        return QString::fromStdString(ret);
    }

    QString currentThreadId() {
        return QString::number((long long)QThread::currentThread(), 16);
    }
    QString md5(const QString& str) {
        return QCryptographicHash::hash(str.toUtf8(),QCryptographicHash::Md5).toHex();
    }

    QStringList syncSuffix() {
        QStringList suffix;
        suffix << ".md";
        suffix << ".docx";
        suffix << ".txt";
        return suffix;
    }

    void moveToCenter(QWidget *source, QWidget *target) {
        if (source->parent() == target) {
            qDebug() <<"source is child of target";
            // 如果有父子关系
            auto g = target->geometry();
            qDebug() << g;
            auto size = source->size();
            qDebug() << source->geometry();
            qDebug() << size;
            int x = (g.width() - size.width()) / 2;
            int y = (g.height() - size.height()) / 2;
            qDebug() << QPoint(x, y);
            source->move(x, y);
        } else {
            auto g = target->geometry();
            qDebug() << g;
            auto size = source->size();
            qDebug() << source->geometry();
            qDebug() << size;
            int x = g.x() + (g.width() - size.width()) / 2;
            int y = g.y() + (g.height() - size.height()) / 2;
            qDebug() << QPoint(x, y);
            source->move(x, y);
        }
    }

}

//
// Created by PikachuHy on 2020/12/25.
//

#include "Utils.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <QThread>
using namespace std;



using namespace std;
std::string chMap = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"s;
namespace Utils {
    std::time_t getTimeStamp() {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now());
        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
        std::time_t timestamp = tmp.count();
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
}
//
// Created by PikachuHy on 2020/12/25.
//

#ifndef MYNOTES_UTILS_H
#define MYNOTES_UTILS_H

#include <QString>
namespace Utils {
    /**
     * 获取当前时间戳
     */
    std::time_t getTimeStamp();
    /**
     * 生成一个唯一的字符串ID
     * @return
     */
    QString generateId();
};


#endif //MYNOTES_UTILS_H

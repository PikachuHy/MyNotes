//
// Created by PikachuHy on 2020/12/25.
//

#ifndef MYNOTES_UTILS_H
#define MYNOTES_UTILS_H

#include <QString>
#include <QFuture>
#include <QTimer>
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
    /**
     * 获取当前进程的ID
     * @return
     */
    QString currentThreadId();

    /**
     * 轮询future是否完成，如果没有完成，反复轮询直到完成。完成时调用callback。
     */

    template<typename T>
    void checkFuture(QFuture<T> future, std::function<void(T)> callback) {
        if (!future.isFinished()) {
            QTimer::singleShot(1000, [future, callback](){
                checkFuture(future, callback);
            });
        } else {
//            callback(future.template result());
            callback(future.result());
        }
    }
};


#endif //MYNOTES_UTILS_H

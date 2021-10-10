//
// Created by PikachuHy on 2020/12/25.
//

#ifndef MYNOTES_UTILS_H
#define MYNOTES_UTILS_H

#include <QString>
#include <QFuture>
#include <QTimer>
class QWidget;
namespace Utils {
    /**
     * 获取当前时间戳
     */
    time_t getTimeStamp();
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

    /**
     * 计算一个字符串的md5，并转成十六进制
     */
    QString md5(const QString& str);

    QStringList syncSuffix();

    /**
     * 移动source到target中间
     * 注意：这个必须是两者的高度宽带都已经算出来的情况下才有效
     */
     void moveToCenter(QWidget* source, QWidget* target);
};


#endif //MYNOTES_UTILS_H

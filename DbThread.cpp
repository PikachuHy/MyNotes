//
// Created by PikachuHy on 2020/12/27.
//

#include "DbThread.h"
#include "DbManager.h"
DbThread::DbThread(const QString &dataPath, QObject *parent) : m_dataPath(dataPath), QThread(parent) {

}

void DbThread::run() {
//    auto db = DbManager(m_dataPath);
    while (true) {

    }
}

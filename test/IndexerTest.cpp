//
// Created by pikachu on 5/19/2021.
//
#include "Indexer.h"
#include <QDebug>
QList<int> testSaveIndex() {
    Indexer indexer("", "test");
    indexer.updateIndex(1, "我是谁？");
    indexer.updateIndex(2, "谁是我？");
    auto ret = indexer.search("我");
    indexer.saveIndex();
    return ret;
}

QList<int> testLoadIndex() {
    Indexer indexer("", "test");
    indexer.loadIndex();
    return indexer.search("我");
}
int main() {
    auto ret1 = testSaveIndex();
    qDebug() << "ret1:" << ret1;
    auto ret2 = testLoadIndex();
    qDebug() << "ret2:" << ret2;
    return 0;
}
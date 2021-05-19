//
// Created by pikachu on 2021/5/15.
//

#ifndef MYNOTES_INDEXER_H
#define MYNOTES_INDEXER_H
#include <QString>
#include <QMap>
namespace index {
    struct Node;
}
class DbManager;
// Indexer 索引器
class Indexer {
public:
    Indexer(DbManager* dbManager);
    // 开始索引
    void startIndex();
    // 索引所有笔记
    void indexAll();
    // 保存索引到磁盘
    void saveIndex();
    // 根据搜索的文本，返回相关的笔记ID
    QList<int> search(QString q);

private:
    QString m_indexPath;
    QMap<int, index::Node*> m_index;
    QStringList m_words;
    QMap<QString, int> m_wordDict;
    DbManager* m_dbManager;
};


#endif //MYNOTES_INDEXER_H

//
// Created by pikachu on 2021/5/15.
//

#ifndef MYNOTES_INDEXER_H
#define MYNOTES_INDEXER_H
#include <QString>
#include <QMap>
namespace index_ns {
    struct Node;
}

namespace QtJieba {
    class Tokenizer;
}
class DbManager;
// Indexer 索引器
class Indexer {
public:
    Indexer(QString  basePath, QString  name);
    // 从磁盘中载入索引
    void loadIndex();
    // 更新索引
    void updateIndex(int id, const QString& text);
    // 保存索引到磁盘
    void saveIndex();
    // 根据搜索的文本，返回相关的笔记ID
    QList<int> search(const QString& q);

    bool isReady();

private:
    QStringList cut(const QString& text);
    void writeDictToDisk();
    void writeIndexToDisk();
    QString dictPath();
    QString indexPath();
private:
    static quint32 m_magic;
    QString m_indexName;
    QString m_indexPath;
    QMap<int, index_ns::Node *> m_index;
    QStringList m_words;
    QMap<QString, int> m_wordDict;
    QtJieba::Tokenizer* m_tokenizer;
    bool m_loading;
};


#endif //MYNOTES_INDEXER_H

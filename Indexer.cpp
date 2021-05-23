//
// Created by pikachu on 2021/5/15.
//

#include "Indexer.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDataStream>
#include <utility>
#include "DbManager.h"
#include "Tokenizer.h"
namespace index {
    struct Node {
        int noteId;
        Node* next;
    };
}
quint32 Indexer::m_magic = (quint32)0xfabfabcd;
Indexer::Indexer(QString basePath, QString name):
    m_tokenizer(nullptr)
    , m_loading(true)
    , m_indexName(std::move(name))
    , m_indexPath(std::move(basePath))
{
    QDir indexDir(m_indexPath);
    if (!indexDir.exists()) {
        qInfo() << "create dir:" << m_indexPath;
        indexDir.mkpath(m_indexPath);
    }
}

void Indexer::loadIndex() {
    qDebug() << "load dict";
    QFile dictFile(dictPath());
    if (dictFile.exists()) {
        bool ok = dictFile.open(QIODevice::ReadOnly | QIODevice::Text);
        if (!ok) {
            qWarning() << "open file fail." << dictFile.fileName();
            return;
        }
        // 读词典
        QString line;
        QTextStream in(&dictFile);
        line = in.readLine();
        while (!line.isNull()) {
            m_wordDict[line] = m_words.size();
            m_words.append(line);
            line = in.readLine();
        }
        dictFile.close();
        qDebug() << "dict total:" << m_words.size();
//        qDebug() << m_words;
    } else {
        qWarning() << "dict file not exist." << dictFile.fileName();
    }
    qDebug() << "load dict done";
    qDebug() << "load index";
    QFile indexFile(indexPath());
    if (indexFile.exists()) {
        bool ok = indexFile.open(QIODevice::ReadOnly);
        if (!ok) {
            qWarning() << "open file fail." << indexFile.fileName();
            return;
        }
        // 读索引文件
        QDataStream in(&indexFile);
        quint32 magic;
        in >> magic;
        if (magic != m_magic) {
            qWarning() << "invalid magic number for index file." << indexFile.fileName();
            return;
        }
        qsizetype total;
        in >> total;
//        qDebug() << "total index:" << total;
        for(int i=0;i<total;i++) {
            index::Node dummy{};
            int count;
            in >> count;
            int wordId = i;
//            qDebug() << m_words[wordId];
//            qDebug() << "word id->count" << wordId << count;
            auto it = &dummy;
            for (int j = 0; j < count; ++j) {
                auto node = new index::Node();
                in >> node->noteId;
//                qDebug() << node->noteId;
                it->next = node;
                it = it->next;
            }
            m_index[wordId] = dummy.next;
        }
        indexFile.close();
    } else {
        qDebug() << "index file not exist." << indexFile.fileName();
    }
    qDebug() << "load index done";
    m_loading = false;
}

void Indexer::saveIndex() {
    writeDictToDisk();
    writeIndexToDisk();
}

QList<int> Indexer::search(const QString& q) {
    qDebug() << "search:" << q;
    // 先忽略其他的语义分析
    auto words = cut(q);
    QList<int> wordIds;
    for(const auto& word: words) {
        if (m_wordDict.contains(word)) {
            wordIds.append(m_wordDict[word]);
        }
    }
    // 求交运算
    if (wordIds.empty()) {
        return {};
    }
    QSet<int> candidates;
    auto wordId = wordIds[0];
    if (!m_index.contains(wordId)) {
        return {};
    }
    auto it = m_index[wordId];
    while (it) {
        candidates.insert(it->noteId);
        it = it->next;
    }
    for(int i=1;i<wordIds.size();i++) {
        QSet<int> tmp;
        auto wordId = wordIds[i];
        auto it = m_index[wordId];
        while (it) {
            auto nodeId = it->noteId;
            if (candidates.contains(nodeId)) {
                tmp.insert(nodeId);
            }
        }
        candidates = tmp;
    }
    auto ret = QList<int>(candidates.begin(), candidates.end());
    qDebug() << "search result:" << ret;
    return ret;
}

bool Indexer::isReady() {
    return !m_loading;
}

void Indexer::updateIndex(int id, const QString &text) {
    auto words = cut(text);
//    qDebug() << "doc:" << id;
//    qDebug() << words;
    for(const auto& word: words) {
        if (word.isEmpty()) continue;
        int wordId;
        if (m_wordDict.contains(word)) {
            wordId = m_wordDict[word];
        } else {
            wordId = static_cast<int>(m_wordDict.size());
            m_wordDict[word] = wordId;
            m_words.append(word);
        }
        auto node = new index::Node();
        node->noteId = id;
        if (m_index.contains(wordId)) {
            auto head = m_index[wordId];
            node->next = head;
            m_index[wordId] = node;
        } else {
            m_index[wordId] = node;
        }
    }
}

QStringList Indexer::cut(const QString &text) {
    if (!m_tokenizer) {
        m_tokenizer = new QtJieba::Tokenizer();
    }
    return m_tokenizer->cut(text);
}

void Indexer::writeIndexToDisk() {
    QFile indexFile(indexPath());
    bool ok = indexFile.open(QIODevice::WriteOnly);
    if (!ok) {
        qWarning() << "open file fail." << indexFile.fileName();
        return;
    }
    // 写索引文件
    QDataStream out(&indexFile);
    out << m_magic;
    out << m_index.size();
//    qDebug() << m_magic << m_index.size();
    for(int i=0;i<m_index.size();i++) {
        auto node = m_index[i];
        auto word = m_words[i];
        int count = 0;
        auto it = node;
        while (it) {
            count++;
            it = it->next;
        }
//        qDebug() << word;
//        qDebug() << "count:" << count;
        out << count;
        it = node;
        while (it) {
//            qDebug() << it->noteId;
            out << it->noteId;
            it = it->next;
        }
    }
    indexFile.close();
}

void Indexer::writeDictToDisk() {

    QFile dictFile(dictPath());
    bool ok = dictFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!ok) {
        qWarning() << "open file fail." << dictFile.fileName();
        return;
    }
    // 写词典
    QTextStream out(&dictFile);
    for(const auto& line: m_words) {
        out << line << '\n';
    }
    dictFile.close();
}

QString Indexer::dictPath() {
    if (m_indexPath.isEmpty()) {
        return QString("%1.dict").arg(m_indexName);
    } else {
        return QString("%1/%2.dict").arg(m_indexPath, m_indexName);
    }
}

QString Indexer::indexPath() {
    if (m_indexPath.isEmpty()) {
        return QString("%1.index").arg(m_indexName);
    } else {
        return QString("%1/%2.index").arg(m_indexPath, m_indexName);
    }
}


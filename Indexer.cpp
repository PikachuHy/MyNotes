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
#include "DbManager.h"
namespace index {
    struct Node {
        int noteId;
        Node* next;
    };
}
Indexer::Indexer(DbManager* dbManager) : m_dbManager(dbManager) {
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    m_indexPath = docPath + "/MyNotes/index";
    QDir indexDir(m_indexPath);
    if (!indexDir.exists()) {
        qInfo() << "create dir:" << m_indexPath;
        indexDir.mkpath(m_indexPath);
    }
}

void Indexer::startIndex() {
    QFile dictFile(m_indexPath + "/note.dict");
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
            m_words.append(line);
            line = in.readLine();
        }
        dictFile.close();
    }
    QFile indexFile(m_indexPath + "/note.index");
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
        if (magic != (quint32)0xfabfabcd) {
            qWarning() << "invalid magic number for index file." << indexFile.fileName();
            return;
        }
        int total;
        in >> total;
        for(int i=0;i<total;i++) {
            index::Node dummy{};
            int wordId;
            in >> wordId;
            int count;
            in >> count;
            auto it = &dummy;
            for (int j = 0; j < count; ++j) {
                auto node = new index::Node();
                in >> node->noteId;
                it->next = node;
                it = it->next;
            }
            m_index[wordId] = dummy.next;
        }
        indexFile.close();
    }
}

void Indexer::indexAll() {
    // 1. 从数据库中读所有的笔记
    // 2. 对每一个笔记进行索引
    // 2.1 对文章进行分词
    // 2.2 建立词到文章的反向映射
    //
    auto cutNote = [](const Note& note) {
        return QStringList();
    };
    auto notes = m_dbManager->getAllNotes();
    for(const auto& note: notes) {
        auto words = cutNote(note);
        for(const auto& word: words) {
            int wordId;
            if (m_wordDict.contains(word)) {
                wordId = m_wordDict[word];
            } else {
                wordId = m_wordDict.size();
                m_wordDict[word] = wordId;
                m_words.append(word);
            }
            auto node = new index::Node();
            node->noteId = note.id();
            if (m_index.contains(wordId)) {
                auto head = m_index[wordId];
                node->next = head;
                m_index[wordId] = node;
            } else {
                m_index[wordId] = node;
            }
        }
    }
}

void Indexer::saveIndex() {

    QFile dictFile(m_indexPath + "/note.dict");
    if (dictFile.exists()) {
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
    QFile indexFile(m_indexPath + "/note.index");
    if (indexFile.exists()) {
        bool ok = indexFile.open(QIODevice::WriteOnly);
        if (!ok) {
            qWarning() << "open file fail." << indexFile.fileName();
            return;
        }
        // 写索引文件
        QDataStream out(&indexFile);
        quint32 magic = (quint32)0xfabfabcd;
        out << magic;
        out << m_index.size();
        for(int i=0;i< m_index.size();i++) {
            auto head = m_index[i];
            int count = 0;
            auto it = head;
            while (it) {
                count++;
                it = it->next;
            }
            out << count;
            it = head;
            while (it) {
                out << it->noteId;
                it = it->next;
            }
        }
        indexFile.close();
    }
}

QList<int> Indexer::search(QString q)
{
    auto cutText = [](const QString& q) {
        return QStringList();
    };
    // 先忽略其他的语义分析
    auto words = cutText(q);
    QList<int> wordIds;
    for(const auto& word: words) {
        if (m_wordDict.contains(word)) {
            wordIds.append(m_wordDict[word]);
        }
    }
    // 求交运算

    return {};
}


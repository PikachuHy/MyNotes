#include "SearchController.h"
#include "SearchDialog.h"
#include "database/DbManager.h"
#include "indexer/Indexer.h"
#include "utils/Utils.h"
#include "utils/Constant.h"
#include <QtConcurrent>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>
#include <QList>
#include <QFuture>

SearchController::SearchController(DbManager* dbManager, Indexer* indexer,
                                   const QString& notesPath, QObject *parent)
    : QObject(parent)
    , m_searchDialog(nullptr)
    , m_dbManager(dbManager)
    , m_indexer(indexer)
    , m_notesPath(notesPath)
{
}

SearchController::~SearchController()
{
    delete m_searchDialog;
}

void SearchController::showSearchDialog(const QRect& parentGeometry)
{
    if (!m_searchDialog) {
        m_searchDialog = new SearchDialog(nullptr);
        // 强制计算搜索框的实际大小
        m_searchDialog->show();
        m_searchDialog->hide();
        connect(m_searchDialog, &SearchDialog::searchTextChanged, this, &SearchController::onSearchTextChanged);
        connect(m_searchDialog, &SearchDialog::clickNote, this, &SearchController::onClickNote);
    }
    auto x = parentGeometry.left() + parentGeometry.width() / 2 - m_searchDialog->width() / 2;
    m_searchDialog->move(x, parentGeometry.top() + Constant::marginToTop);
    m_searchDialog->show();
}

void SearchController::hideSearchDialog()
{
    if (m_searchDialog) {
        m_searchDialog->hide();
    }
}

void SearchController::initIndexer()
{
    auto readNoteContent = [](const Note& note) -> QString {
        auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
        QString notePath = docPath + "/MyNotes/workshop/" + note.strId() + "/index.md";
        QFile noteFile(notePath);
        if (!noteFile.exists()) {
            qWarning() << "note not exist." << notePath;
            return "";
        }
        bool ok = noteFile.open(QIODevice::ReadOnly);
        if (!ok) {
            qWarning() << "file open fail." << notePath;
            return "";
        }
        return noteFile.readAll();
    };
    auto f = [this, &readNoteContent]() {
        auto notes = m_dbManager->getAllNotes();
        int count = 0;
        for(const auto& note: notes) {
            auto content = readNoteContent(note);
            m_indexer->updateIndex(note.id(), content);
            count++;
            qDebug() << count << "/" << notes.size();
        }
        m_indexer->saveIndex();
        return 0;
    };
    auto callback = [](int) {
        qDebug() << "index all done!";
    };
    auto ret = QtConcurrent::run(f);
    Utils::checkFuture<int>(ret, callback);
}

void SearchController::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        qDebug() << "ignore empty search string";
        return;
    }
    qDebug() << "search" << text;
    auto f = [this](const QString& text) -> QList<Note> {
        qDebug() << "do searching";
        auto noteIds = this->m_indexer->search(text);
        QList<Note> noteList;
        for(auto noteId: noteIds) {
            auto note = m_dbManager->getNote(noteId);
            if (note.id() != -1) {
                noteList.push_back(note);
            }
        }
        return noteList;
    };
    auto callback = [this](const QList<Note> &noteList) {
        Search::SearchResult ret;
        for(const auto& note: noteList) {
            Search::SearchResultItem item;
            item.noteId = note.id();
            item.noteTitle = note.title();
            // 获得笔记的完整路径
            int pathId = note.pathId();
            QStringList paths;
            while (pathId != 0) {
                auto path = m_dbManager->getPath(pathId);
                paths.append(path.name());
                pathId = path.parentId();
            }
            item.paths = QStringList (paths.rbegin(), paths.rend());
            ret.items.emplace_back(item);
        }
        this->m_searchDialog->setSearchResult(ret);
    };
    auto ret = QtConcurrent::run(f, text);
    Utils::checkFuture<QList<Note>>(ret, callback);
}

void SearchController::onClickNote(int noteId)
{
    emit noteSelected(noteId);
    if (m_searchDialog) {
        m_searchDialog->hide();
    }
}

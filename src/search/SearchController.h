#ifndef MYNOTES_SEARCHCONTROLLER_H
#define MYNOTES_SEARCHCONTROLLER_H

#include <QObject>
#include <QString>
#include <QRect>

class SearchDialog;
class DbManager;
class Indexer;
class QWidget;

namespace Search {
    struct SearchResult;
}

class SearchController : public QObject {
    Q_OBJECT
public:
    explicit SearchController(DbManager* dbManager, Indexer* indexer,
                              const QString& notesPath, QObject *parent = nullptr);
    ~SearchController() override;

    void showSearchDialog(const QRect& parentGeometry);
    void hideSearchDialog();
    void initIndexer();

signals:
    void noteSelected(int noteId);

private slots:
    void onSearchTextChanged(const QString& text);
    void onClickNote(int noteId);

private:
    SearchDialog* m_searchDialog;
    DbManager* m_dbManager;
    Indexer* m_indexer;
    QString m_notesPath;
};

#endif

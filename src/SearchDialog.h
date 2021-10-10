//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_SEARCHDIALOG_H
#define MYNOTES_SEARCHDIALOG_H

#include <QDialog>
#include <QListView>
#include <utility>
#include <vector>
#include <QMetaType>
class QLineEdit;
namespace Search {
    struct Highlight {
        QString keyword;
        QString text;
    };
    struct SearchResultItem {
        int noteId = -1;
        QString noteTitle;
        std::vector<Highlight> highlights;
        QStringList paths;
        SearchResultItem() = default;
    };
    struct SearchResult {
        std::vector<SearchResultItem> items;
        bool empty() const { return items.empty(); }
    };
    class ListView;
}
Q_DECLARE_METATYPE(Search::SearchResultItem)
class SearchResultItemDelegate;
class QLabel;
class SearchDialog: public QDialog {
Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = nullptr);
    void setSearchResult(const Search::SearchResult &searchResult);
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

signals:
    void searchTextChanged(const QString& text);
    void clickNote(int noteId);
public slots:
    void on_lineEdit_textChanged(const QString &text);
private:
    QLineEdit* m_lineEdit;
    Search::ListView* m_listView;
    SearchResultItemDelegate* m_itemDelegate;
};


#endif //MYNOTES_SEARCHDIALOG_H

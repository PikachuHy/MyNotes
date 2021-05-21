//
// Created by PikachuHy on 2020/12/27.
//

#include "SearchDialog.h"
#include "Constant.h"
#include "Utils.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLabel>
#include <QMovie>
namespace Search {
    class ListView : public QListView {
    Q_OBJECT
    public:
        explicit ListView(QWidget *parent = nullptr);
        void showLoading() {
            Utils::moveToCenter(m_loading, this);
            m_loading->show();
        }
        void hideLoading() {
            m_loading->hide();
        }
        void showNoResults() {
            m_noResults = true;
        }
    protected:
        void paintEvent(QPaintEvent *e) override;
        void keyPressEvent(QKeyEvent *event) override;

    private:
        QLabel* m_loading;
        bool m_noResults;
    };

    ListView::ListView(QWidget *parent) : QListView(parent) {
//        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//        setMinimumWidth(Constant::searchDialogWidth);
        setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_loading = new QLabel(this);
//    m_loading->setWindowFlag(Qt::FramelessWindowHint);
        auto movie = new QMovie(Constant::loadingImagePath);
        movie->start();
        m_loading->resize(QPixmap(Constant::loadingImagePath).size());
        m_loading->setMovie(movie);
//    m_loading->move(250, 109);
        m_loading->hide();
        m_noResults = false;
    }

    void ListView::keyPressEvent(QKeyEvent *event) {
        QAbstractItemView::keyPressEvent(event);
    }

    void ListView::paintEvent(QPaintEvent *e) {
        QListView::paintEvent(e);
        if (model() && model()->rowCount() > 0) return;
        if (m_noResults) {
            QPainter painter(viewport());
            painter.drawText(rect(), Qt::AlignCenter, tr("No Results"));
        }
    }
}
class SearchResultItemDelegate : public QStyledItemDelegate
{
public:
    explicit SearchResultItemDelegate(QObject *parent = nullptr) {

    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (!index.isValid()) {
            return;
        }
        painter->save();
        if (option.state.testFlag(QStyle::State_MouseOver)) {
            // 如果鼠标悬停
            painter->fillRect(option.rect, QColor("#ebeced"));
        }
        auto item = index.data(Qt::UserRole+1).value<Search::SearchResultItem>();
        painter->drawPixmap(option.rect.topLeft(), QPixmap(":icon/note_64x64.png").scaled(32, 32));
        QRect textRect(option.rect.left() + 40, option.rect.top(), option.rect.width() - 40, 32);
        painter->drawText(textRect, Qt::AlignVCenter, item.noteTitle);
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        return QStyledItemDelegate::sizeHint(option, index);
    }
private:
};
SearchDialog::SearchDialog(QWidget *parent): QDialog(parent) {
    setWindowFlags(Qt::Dialog
    | Qt::FramelessWindowHint
//    | Qt::Popup
    );
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setMinimumSize(Constant::searchDialogWidth, Constant::searchDialogHeight);
    m_listView = new Search::ListView();
    m_itemDelegate = new SearchResultItemDelegate;
    m_listView->setItemDelegate(m_itemDelegate);
    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(m_lineEdit);
    vbox->addWidget(m_listView);
    setLayout(vbox);
    connect(m_lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
        m_listView->showLoading();
        emit on_lineEdit_textChanged(text);
    });
    connect(m_listView, &QListView::clicked, [this](const QModelIndex &index) {
        auto item = index.data(Qt::UserRole+1).value<Search::SearchResultItem>();
//        qDebug() << "click" << noteId;
        emit clickNote(item.noteId);
    });
}

void SearchDialog::on_lineEdit_textChanged(const QString &text) {
    emit searchTextChanged(text);
}


void SearchDialog::hideEvent(QHideEvent *event) {
    m_lineEdit->clear();
    m_listView->setModel(nullptr);
    QWidget::hideEvent(event);
}

void SearchDialog::setSearchResult(const Search::SearchResult &searchResult) {
    m_listView->hideLoading();
    if (searchResult.empty()) {
        m_listView->showNoResults();
    }
    auto model = new QStandardItemModel(this);
    int count = 0;
    for(const auto& researchResultItem: searchResult.items) {
        count++;
        const QString &title = QString::number(count) + ". " + researchResultItem.noteTitle;
        auto item = new QStandardItem(title);
        // 设置不可编辑
        item->setEditable(false);
        // 设置图标
        item->setData(QPixmap(":icon/note_64x64.png").scaled(32, 32), Qt::DecorationRole);
//        qDebug() << "note id" << researchResultItem.noteId;
        item->setData(QVariant::fromValue(researchResultItem), Qt::UserRole + 1);
        auto noteId = item->data(Qt::UserRole+1).toInt();
//        qDebug() << noteId;
        model->appendRow(item);
    }
    m_listView->setModel(model);
    // 计算列表的高度
    m_listView->setMinimumHeight(600);
    m_listView->show();
}

void SearchDialog::showEvent(QShowEvent *event) {
    // 必须先激活，然后focus才能直接输入
    this->m_listView->activateWindow();
    this->m_lineEdit->setFocus();
}
#include "SearchDialog.moc"

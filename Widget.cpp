#include "Widget.h"
#include "FileTreeModel.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "TreeView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFile>
#include <QHeaderView>
#include <QMenu>
#include <QEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

Widget::Widget(QWidget *parent)
        : QWidget(parent) {
    m_treeView = new TreeView();
    m_textEdit = new QTextEdit();
    m_textPreview = new QTextBrowser();
    m_titleLabel = new QLabel("untitled");
    m_titleLineEdit = new QLineEdit();
    m_toggleEditAndPreview = new QPushButton("toggle");
    m_showEditButton = new QPushButton("Edit");
    m_showPreviewButton = new QPushButton("Preview");
    // 处理Ctrl+S保存
    m_textEdit->installEventFilter(this);
    auto splitter = new QSplitter();
    splitter->addWidget(m_treeView);
    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addLayout(initTitleLayout());
    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 0, 0, 0);
//    hbox->addWidget(m_treeView);
    hbox->addWidget(m_textEdit);
    hbox->addWidget(m_textPreview);
    vbox->addLayout(hbox);
    auto w = new QWidget();
    w->setLayout(vbox);

    splitter->addWidget(w);

    splitter->setParent(this);

    auto mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
    m_notesPath = "/Users/pikachu/Documents/MyNotes";
    m_treeModel = new TreeModel(m_notesPath);
    m_treeView->setModel(m_treeModel);
    initSlots();
    resize(1200, 800);
    auto _font = font();
    _font.setPointSize(16);
    setFont(_font);
}

Widget::~Widget() {
}

QLayout *Widget::initTitleLayout() {
    auto hbox = new QHBoxLayout();
    hbox->addWidget(m_titleLabel);
    hbox->addStretch(1);
    hbox->addWidget(m_toggleEditAndPreview);
    hbox->addWidget(m_showEditButton);
    hbox->addWidget(m_showPreviewButton);
    return hbox;
}

void Widget::on_treeView_activated(const QModelIndex &index) {
    if (!index.isValid()) return;
    auto item = static_cast<TreeItem *>(index.internalPointer());
    if (item->isFile()) {
        m_titleLabel->setText(item->path().mid(m_notesPath.size()));
        m_curNotePath = item->path();
        QFile file(item->path());
        file.open(QIODevice::ReadOnly);
        QString mdText = file.readAll();
        m_textEdit->setText(mdText);
        m_textPreview->setMarkdown(mdText);
        file.close();
    }
}

void Widget::initSlots() {
    connect(m_treeView, &QTreeView::pressed, this, &Widget::on_treeView_activated);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &Widget::on_treeView_customContextMenuRequested);
//    connect(m_textEdit, &QTextEdit:)
}

void Widget::on_treeView_customContextMenuRequested(const QPoint &pos) {
    QMenu menu;
    auto index = m_treeView->indexAt(pos);
    if (index.isValid()) {
        auto item = static_cast<TreeItem *>(index.internalPointer());
        if (item->isTrashItem()) {
            auto a = new QAction("Clear Trash", &menu);
            menu.addAction(a);
        } else if (item->isAttachmentItem()) {

        } else if (item->isFile()) {
            auto a = new QAction("Trash Note", &menu);
            menu.addAction(a);
        } else {
            auto a = new QAction("New Note", &menu);
            menu.addAction(a);
            auto b = new QAction("New Folder", &menu);
            menu.addAction(b);
        }
    }
    menu.exec(m_treeView->mapToGlobal(pos));
}

bool Widget::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::KeyPress) {
        auto *event = static_cast<QKeyEvent *>(e);
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            if (m_curNotePath.isEmpty()) {
                return true;
            }
            QFile file(m_curNotePath);
            file.open(QIODevice::WriteOnly);
            const QString &mdText = m_textEdit->toPlainText();
            file.write(mdText.toUtf8());
            file.close();
            m_textPreview->setMarkdown(mdText);
            return true;
        }
        if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier)) {
            qDebug() << "paste";
            auto mimeData = QApplication::clipboard()->mimeData();
            if (mimeData->hasImage()) {
                qDebug() << "image";
                auto pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
                QString savePath = attachmentPath() + "/1.png";
                auto ret = pixmap.save(savePath);
                if (!ret) {
                    qDebug() << "save to" << savePath << "fail";
                }

            } else if (mimeData->hasHtml()) {
                qDebug() << "html";
            } else if (mimeData->hasText()) {
                qDebug() << "text";
            } else {
                qDebug() << "other";
            }
        }
    }
    return QObject::eventFilter(watched, e);
}


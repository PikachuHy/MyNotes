#include "Widget.h"
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
#include <iostream>
#include <fstream>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include "markdown.h"
#include "Utils.h"

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

void Widget::on_treeView_pressed(const QModelIndex &index) {
    if (!index.isValid()) return;
    auto item = static_cast<TreeItem *>(index.internalPointer());
    if (item->isFile()) {
        m_titleLabel->setText(item->path().mid(m_notesPath.size()));
        m_curNotePath = item->path();
        loadMdText();
        updatePreview();
    }
}

void Widget::initSlots() {
    connect(m_treeView, &QTreeView::pressed, this, &Widget::on_treeView_pressed);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &Widget::on_treeView_customContextMenuRequested);
//    connect(m_textEdit, &QTextEdit:)
}

void Widget::on_treeView_customContextMenuRequested(const QPoint &pos) {
    QMenu menu;
    auto index = m_treeView->indexAt(pos);
    if (index.isValid()) {
        auto item = static_cast<TreeItem *>(index.internalPointer());
        m_curItem = item;
        m_curCheckedPath = item->path();
        m_curIndex = index;
        if (item->isTrashItem()) {
            auto a = new QAction("Clear Trash", &menu);
            menu.addAction(a);
        } else if (item->isAttachmentItem()) {

        } else if (item->isFile()) {
            auto a = new QAction("Trash Note", &menu);
            menu.addAction(a);
            connect(a, &QAction::triggered, this, &Widget::on_action_trashNote);
        } else {
            auto a = new QAction("New Note", &menu);
            menu.addAction(a);
            connect(a, &QAction::triggered, this, &Widget::on_action_newNote);
            auto b = new QAction("New Folder", &menu);
            connect(b, &QAction::triggered, this, &Widget::on_action_newFolder);
            menu.addAction(b);
        }
    }
    menu.exec(m_treeView->mapToGlobal(pos));
}

bool Widget::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::KeyPress) {
        auto *event = static_cast<QKeyEvent *>(e);
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            saveMdText();
            updatePreview();
            return true;
        }
        if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier)) {
            qDebug() << "paste";
            auto mimeData = QApplication::clipboard()->mimeData();
            if (mimeData->hasImage()) {
                qDebug() << "image";
                auto pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
                QString saveName = "image-" + Utils::generateId() + ".png";
                QString savePath = attachmentPath() + saveName;
                auto ret = pixmap.save(savePath);
                if (!ret) {
                    qDebug() << "save to" << savePath << "fail";
                }
                m_textEdit->insertPlainText(imageMdText(saveName));
                saveMdText();
                updatePreview();
                return true;
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

void Widget::updatePreview() {
    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open(m_curNotePath.toStdString());
    ofile.open(tmpHtmlPath().toStdString());
    std::istream *in=&ifile;
    markdown::Document doc;
    doc.read(*in);
    doc.write(ofile);
    ofile.close();
    ifile.close();
    QFile file(tmpHtmlPath());
    file.open(QIODevice::ReadOnly);
    QString html = file.readAll();
    std::cout << html.toStdString() << std::endl;
    m_textPreview->setHtml(html);
}

void Widget::saveMdText() {
    if (m_curNotePath.isEmpty()) {
        return;
    }
    QFile file(m_curNotePath);
    file.open(QIODevice::WriteOnly);
    const QString &mdText = m_textEdit->toPlainText();
    file.write(mdText.toUtf8());
    file.close();
}

void Widget::on_action_newNote() {
    bool ok;
    QString noteName = QInputDialog::getText(this, tr("New Note"),tr("Note name"), QLineEdit::Normal,"untitled", &ok);
    qDebug() << "new note name:" << noteName;
    if (ok && !noteName.isEmpty()) {
        QString newNotePath = m_curCheckedPath + "/" + noteName;
        QFile file = QFile(newNotePath);
        if (file.exists()) {
            QMessageBox::warning(this, tr("warning"), tr("Note already exist!"));
        } else {
            qDebug() << "create new note:" << noteName;
            ok = file.open(QIODevice::WriteOnly);
            if (!ok) {
                qDebug() << "open file fail:" << newNotePath;
            } else {
                QList<QVariant> data;
                data << noteName;
                auto item = new TreeItem(data, m_curItem);
                item->setPath(newNotePath);
                auto newNoteIndex = m_treeModel->addNewNode(m_curIndex, item);
                m_treeView->setCurrentIndex(newNoteIndex);
                QString newNoteText = "# " + noteName;
                file.write(newNoteText.toUtf8());
                file.close();
                m_curNotePath = newNotePath;
                loadMdText();
                updatePreview();
            }
        }
    }
}

void Widget::on_action_newFolder() {

    bool ok;
    QString folderName = QInputDialog::getText(this, tr("New Folder"), tr("Folder name"),
                                               QLineEdit::Normal, "folder", &ok);
    qDebug() << "new folder name:" << folderName;
    if (!ok || folderName.isEmpty()) return;
    QString newFolderPath = m_curCheckedPath + "/" + folderName;
    QDir dir(newFolderPath);
    if (dir.exists()) {
        QMessageBox::warning(this, tr("warning"), tr("Folder already exist!"));
        return;
    }
    qDebug() << "create new folder:" << folderName;
    dir.mkpath(newFolderPath);
    QList<QVariant> data;
    data << folderName;
    auto item = new TreeItem(data, m_curItem);
    item->setPath(newFolderPath);
    auto newNoteIndex = m_treeModel->addNewFolder(m_curIndex, item);
    m_treeView->setCurrentIndex(newNoteIndex);
}

void Widget::loadMdText() {
    QFile file(m_curNotePath);
    file.open(QIODevice::ReadOnly);
    QString mdText = file.readAll();
    m_textEdit->setText(mdText);
    file.close();
}

void Widget::on_action_trashNote() {
    if (!m_curIndex.isValid()) {
        return;
    }
    auto item = static_cast<TreeItem *>(m_curIndex.internalPointer());
    const QString noteOldPath = item->path();
    QString noteTrashPath = trashPath() + QFileInfo(noteOldPath).fileName();
    qDebug() << "trash" << noteOldPath << "to" << noteTrashPath;
    bool ret = QFile::rename(noteOldPath, noteTrashPath);
    if (!ret) {
        qDebug() << "trash" << noteOldPath << "success";
        QMessageBox::critical(this, tr("trash note"), tr("trash note fail"));
        return;
    }
    m_treeModel->removeNode(m_curIndex);
    qDebug() << "trash" << noteOldPath << "success";
}

void Widget::on_action_trashFolder() {

}


#include "Widget.h"
#include "SyncService.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "TreeView.h"
#include "DbManager.h"
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
#include "Utils.h"
#include <QtSql>
#include <QtConcurrent>
//#include "cppjieba/Jieba.hpp"
#include <unordered_set>
#include <QLineEdit>
#include "SearchDialog.h"
#include <QtWidgets>
#include "ListModel.h"
#include "ListView.h"
#include "Constant.h"
#include <vector>
#include <QFuture>
#include <functional>
#include "Toast.h"
#include "ChooseFolderWidget.h"
#include "ElasticSearchRestApi.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include "Http.h"
#include <QDesktopServices>
#include <QStringList>
#include "FileSystemWatcher.h"
#include "AboutDialog.h"
#include <QSplitter>
#ifdef ENABLE_TROJAN
#include "TrojanThread.h"
#endif
#include "TabWidget.h"
#include "TextPreview.h"
#include <QVector>
#include <QApplication>
#include "Indexer.h"
#include "HtmlExporter.h"
Widget::Widget(QWidget *parent)
        : PiWidget(parent),
        m_showOpenInTyporaTip(true),
        m_settings(Settings::instance())
        , m_fileSystemWatcher(FileSystemWatcher::instance())
        , m_systemTrayIcon(new QSystemTrayIcon(this))
        , m_timer(new QTimer(this))
//        , m_indexer(new Indexer())
#ifdef ENABLE_TROJAN
        , m_trojanThread(nullptr)
#endif
//        ,m_jieba(nullptr)
        {
    initSystemTrayIcon();
    initFileSystemWatcher();
    if (!Settings::instance()->modeOffline) {
        // 一天
        m_timer->setInterval(1000 * 60 * 60 * 24);
        m_timer->start();
        connect(m_timer, &QTimer::timeout, [this]() {
            qInfo() << "24h sync";
            m_syncService->syncAllWatching();
            m_syncService->updateProfile();
        });
    }
    m_treeView = new TreeView();
    m_textEdit = new QTextEdit();
    m_tabWidget = new TabWidget();
    connect(m_tabWidget, &TabWidget::tabCloseRequested, [this](int index){
        m_tabWidget->removeTab(index);
    });
//    m_textPreview = new WebEngineView();
//    connect(m_textPreview, &WebEngineView::urlChanged, [this](const QUrl &url){
//        if (url.toString().startsWith("http://in.css518.cn/")) {
//            m_textPreview->setUrl(url);
//        } else {
//            qWarning() << "not allow:" << url;
//        }
//    });
    // 处理Ctrl+S保存
    m_treeView->installEventFilter(this);
    m_treeView->setMinimumWidth(300);
//    initShortcut();
    auto mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(m_treeView);
    splitter->addWidget(m_tabWidget);
    splitter->setStretchFactor(1, 3);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
    setWindowIcon(QIcon(QPixmap(":/icon/notebook_128x128.png")));
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    m_notesPath = docPath + "/MyNotes/";
    m_htmlExporter = new HtmlExporter(workshopPath(), this);
    {
        auto esApi = new ElasticSearchRestApi(this);
        m_syncService = new SyncService(m_notesPath, esApi, m_htmlExporter, m_dbManager, this);
    }
    if (!QFile(m_notesPath).exists()) {
        qDebug() << "mkdir" << m_notesPath;
        QDir().mkdir(m_notesPath);
    }
    m_indexer = new Indexer(m_notesPath+"index", "note");
    m_indexer->loadIndex();
    m_dbManager = new DbManager(m_notesPath, this);
    m_treeModel = new TreeModel(m_notesPath, m_dbManager);
    m_treeView->setModel(m_treeModel);
    // initIndexer();
    initSlots();
    auto _font = font();
#ifdef Q_OS_WIN
    _font.setPointSize(12);
    _font.setFamily("微软雅黑");
#else
    _font.setPointSize(16);
#endif
    setFont(_font);
    m_lastPressShiftTime = 0;
    m_maxShiftInterval = 200;
    m_listModel = new ListModel(this);
    m_searchDialog = nullptr;
    m_listView = nullptr;
    // 读最后一次打开的笔记
    loadLastOpenedNote();
    if (!Settings::instance()->modeOffline) {
        const int syncVersion = 20210402;
        int curSyncVersion = Settings::instance()->syncVersion;
        if (curSyncVersion < syncVersion) {
            QTimer::singleShot(1000, [this, syncVersion]() {
                qInfo() << "reupload note for new sync version";
                m_syncService->syncAll();
                m_syncService->syncAllWatching();
                Settings::instance()->syncVersion = syncVersion;
                qInfo() << "reupload note done.";
            });
        }
        QTimer::singleShot(1000, [this]() {
            m_syncService->updateProfile();
        });
    }
    // 设置开启自启动
    setAutoStart();
}

void Widget::loadLastOpenedNote() {
    QTimer::singleShot(50, [this](){
        QString lastOpenNotePath = m_settings->lastOpenNotePath;
        if (lastOpenNotePath.isEmpty()) {
            qDebug() << "no last note";
            return ;
        }
        if (!QFile(lastOpenNotePath).exists()) {
            qInfo() << "last open note not exist." << lastOpenNotePath;
            return;
        }
//        if (lastOpenNotePath.startsWith(workshopPath())) {
//            //
//        } else {
//            // 监控文件夹的情况
//        }
        loadNote(lastOpenNotePath);
//        auto lastNoteId = m_settings->value("last_note", -1).toInt();
//        if (lastNoteId != -1) {
//            auto lastNote = m_dbManager->getNote(lastNoteId);
//            if (lastNote.id() != -1) {
//                qDebug() << "load last note";
//                loadNote(lastNote);
//            } else {
//                qWarning() << "no last note id error:" << lastNoteId;
//            }
//        } else {
//            qDebug() << "no last note";
//        }
    });
}

Widget::~Widget() {
    delete m_searchDialog;
    delete m_listView;
}

void Widget::on_treeView_pressed(const QModelIndex &index) {
    if (!index.isValid()) return;
    auto item = static_cast<TreeItem *>(index.internalPointer());
    auto tryLoadNote = [this](const QString& notePath) {
        qDebug() << "note path: " << notePath;
        // 右键选中笔记时，如果当前笔记就是选中的笔记，不重新载入笔记内容
        if (notePath == currentNotePath()) {
            return;
        }
        if (!notePath.isEmpty()) {
            loadNote(notePath);
        } else {
            qDebug() << "note path is empty.";
            showErrorDialog(tr("note path is empty."));
        }
    };
    QString notePath;
    if (item->isWatchingFileItem()) {
        notePath = item->path();
        tryLoadNote(notePath);
    } else if (item->isNoteItem()) {
        // 如果是workshop的note
        auto noteItem = static_cast<NoteItem*>(item);
        const auto& note = noteItem->note();
        notePath = noteRealPath(note);
        tryLoadNote(notePath);
    }
}

void Widget::initSlots() {
    connect(m_treeView, &QTreeView::pressed, this, &Widget::on_treeView_pressed);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &Widget::on_treeView_customContextMenuRequested);
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

        } else if (item->isWatchingFolderItem()) {
            menu.addAction(tr("Sync Folder"), [this, item]() {
                m_syncService->syncWatchingFolder(item->path());
                m_syncService->showSyncResult(tr("Sync Watching Folder Success"));
            });
            // 如果当前文件夹是监控文件夹的根目录
            if (item->parentItem()->isWatchingItem()) {
                menu.addAction(tr("Remove Watching Folder"), [this, item](){
                    qDebug() << "remove watching folder: " << item->path();
                    m_treeModel->removeNode(m_treeView->currentIndex());
                    QStringList watchingDirs = Settings::instance()->watchingFolders;
                    watchingDirs.removeOne(item->path());
                    Settings::instance()->watchingFolders = watchingDirs;
                });
            }
        } else if (item->isWatchingFileItem()) {
            menu.addAction(tr("Sync Note"), [this, item]() {
                m_syncService->syncWatchingFile(item->path());
                m_syncService->showSyncResult(tr("Sync Watching Note Success"));
            });
        } else if (item->isWatchingItem()) {
            menu.addAction(tr("Add Watch Folder"), [this]() {
                qDebug() << "watch folder";
                auto dir = QFileDialog::getExistingDirectory(this, tr("Add Watch Folder"));
                QStringList watchingDirs = Settings::instance()->watchingFolders;
                bool watchSuccess = true;
                for (auto watchingDir: watchingDirs) {
                    if (dir.startsWith(watchingDir + '/')) {
                        watchSuccess = false;
                        showErrorDialog(tr("Folder %1 has already in watching.").arg(dir));
                        break;
                    }
                }
                if (watchSuccess) {
                    watchingDirs.append(dir);
                    Settings::instance()->watchingFolders = watchingDirs;
                    qDebug() << "add " << dir << "to watching";
                    m_treeModel->addWatchingDir(m_treeView->currentIndex(), dir);
                    // 添加的监控文件夹要自动同步
                    m_syncService->syncWatchingFolder(dir);
                    m_syncService->updateProfile();
                    this->showInfo(tr("Sync Folder"), tr("Sync Folder Done."));
                }
            });
            menu.addAction(tr("Sync All"), [this]() {
                qDebug() << "sync all watching item";
                m_syncService->syncAllWatching();
            });
        } else if (item->isFile()) {
            auto a = new QAction("Open in Typora", &menu);
            menu.addAction(a);
            connect(a, &QAction::triggered, this, &Widget::on_action_openInTypora);
            auto b = new QAction("Trash Note", &menu);
            menu.addAction(b);
            connect(b, &QAction::triggered, this, &Widget::on_action_trashNote);
            auto c = new QAction("Export to HTML", &menu);
            menu.addAction(c);
            connect(c, &QAction::triggered, this, &Widget::on_action_exportNoteToHTML);
            menu.addAction(tr("Add to ..."), [this](){
                this->addNoteTo();
            });
            auto noteItem = (NoteItem*)item;
            auto note = noteItem->note();
            auto notePath = noteRealPath(note);
            QUrlQuery urlQuery;
            urlQuery.addQueryItem("note", note.strId());
            urlQuery.addQueryItem("path", QString::number(note.pathId()));

            auto url = QString("note://localhost?%1").arg(urlQuery.toString());
            auto mdLink = QString("[%1](%2)").arg(note.title()).arg(url);
            auto submenu = new QMenu("Copy");
            menu.addMenu(submenu);
            submenu->addAction(tr("Copy Path: \"%1\"").arg(notePath.replace("&", "&&"))
                           , [this, notePath]() {
                QApplication::clipboard()->setText(notePath);
            });
            submenu->addAction(tr("Copy URL: \"%1\"").arg(url.replace("&", "&&")),
                           [this, url]() {
                QApplication::clipboard()->setText(url);

            });
            submenu->addAction(tr("Copy Markdown Link: \"%1\"").arg(mdLink.replace("&", "&&")),
                           [this, mdLink]() {
                QApplication::clipboard()->setText(mdLink);
            });
        } else {
            auto a = new QAction("New Note", &menu);
            menu.addAction(a);
            connect(a, &QAction::triggered, this, &Widget::on_action_newNote);
            auto b = new QAction("New Folder", &menu);
            connect(b, &QAction::triggered, this, &Widget::on_action_newFolder);
            menu.addAction(b);
            if (!item->isWorkshopItem()) {
                auto c = new QAction("Trash Folder", &menu);
                connect(c, &QAction::triggered, this, &Widget::on_action_trashFolder);
                menu.addAction(c);
                c->setEnabled(item->childCount() == 0);
            }
            if (item->isWorkshopItem()) {
                menu.addAction(tr("Sync All"), [this](){
                    m_syncService->syncAll();
                });
            }
        }
        // 除了假的监控结点外，其他的结点都可以在文件浏览器中打开
        if (!(item->isWatchingItem() || item->isFolderItem())) {
            menu.addAction(
#ifdef Q_OS_MAC
                    tr("Open in Finder"),
#else
                        tr("Open in Explore"),
#endif
                    [this, item]() {
                        auto note = this->m_dbManager->getNote(item->path());
                        QString url = this->noteRealPath(note)+"/..";
                        QDesktopServices::openUrl(QUrl(url));
                    }
            );
        }
    }
    menu.exec(m_treeView->mapToGlobal(pos));
}

bool Widget::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::KeyPress) {
        auto *event = static_cast<QKeyEvent *>(e);
        if (event->key() == Qt::Key_Comma && (event->modifiers() & Qt::ControlModifier)) {
            SettingsDialog dialog;
            dialog.exec();
        }
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            saveMdText();
            updatePreview();
            return true;
        }
        /*
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
        */
        // 按e进入编辑
        if (event->key() == Qt::Key_E) {
            openInTypora(currentNotePath());
        }
        if (watched == m_treeView) {
            if (event->key() == Qt::Key_Backspace) {
                auto item = static_cast<TreeItem*>(m_treeView->currentIndex().internalPointer());
                if (item->isFile()) {
                    auto ret = QMessageBox::question(this, tr("Trash Note"),
                                                     tr("Trash Note?"),
                                                     QMessageBox::Yes|QMessageBox::No,
                                                     QMessageBox::Yes);
                    if (ret == QMessageBox::Yes) {
                        on_action_trashNote();
                        return true;
                    }
                }
            }
        }

        if (event->key() == Qt::Key_Shift) {
            auto curTime = Utils::getTimeStamp();
            if (curTime - m_lastPressShiftTime < m_maxShiftInterval) {
                m_lastPressShiftTime = curTime;
                if (!m_searchDialog) initSearchDialog();
                auto x = this->geometry().left() + this->geometry().width() / 2 - m_searchDialog->width() / 2;
                m_searchDialog->move(x, this->geometry().top() + Constant::marginToTop);
                m_searchDialog->show();
            } else {
                m_lastPressShiftTime = curTime;
            }
        }

    }
    return QObject::eventFilter(watched, e);
}

void Widget::closeEvent(QCloseEvent *event)
{
#ifdef _DEBUG
    qDebug() << "close window in debug mode";
#else
    // 这个在Debug的时候很讨厌
    if (m_systemTrayIcon->isVisible()) {
        hide();
        event->ignore();
    }
#endif
}

void Widget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    // FIXME: 这个地方写了会出问题，最小化窗口后，再点击托盘不显示窗口
//    if (m_systemTrayIcon->isVisible()) {
//        hide();
//        event->ignore();
//    }
}

void Widget::updatePreview() {
    updatePreview(currentNotePath());
}

void Widget::updatePreview(const QString& path) {
    auto tabs = m_tabWidget->tabs();
    for(auto tab: tabs) {
        if (tab->filePath() == path) {
            m_tabWidget->setCurrentWidget(tab);
            return;
        }
    }
    TextPreview* textPreview = new TextPreview();
    connect(textPreview, &TextPreview::linkClicked, [this](QString link){
        QUrl url(link);
        auto scheme = url.scheme();
        if (scheme != "note") {
            return;
        }
        auto q = url.query();
        qDebug() << "query" << q;
        QUrlQuery urlQuery(q);
        auto noteStrId = urlQuery.queryItemValue("note");
        qDebug() << "note id" << noteStrId;
        auto pathId = urlQuery.queryItemValue("path");
        auto note = this->m_dbManager->getNote(noteStrId);
        if (note.id() == -1) {
            showWarning(tr("Open Note"), tr("Note not exist.\nNote Str ID: %1").arg(noteStrId));
        } else {
            loadNote(note);
        }
    });
    textPreview->loadFile(path);
    QString title = QFileInfo(path).fileName();
    if (path.startsWith(workshopPath())) {
        auto strId = getWorkshopNoteStrIdFromPath(path);
        auto note = m_dbManager->getNote(strId);
        title = note.title();
    }
    m_tabWidget->add(textPreview, title);
#if 0
    QFile mdFile(path);
    mdFile.open(QIODevice::ReadOnly);
    Document doc(mdFile.readAll());
    auto html = doc.toHtml();
    QFile htmlFile(tmpHtmlPath());
    htmlFile.open(QIODevice::WriteOnly);
//    QString mdCssPath = tmpPath() + "github-markdown.css";
    QString mdCssPath = "qrc:///css/github-markdown.css";
//    QString mdCssPath = "qrc:///css/github.css";
    auto allHtml = R"(<!DOCTYPE html><html><head>
<meta charset="utf-8">
<title>Markdown</title>
<link rel="stylesheet" href=")"
+
mdCssPath
+
R"(">
</head>
<body>
<article class="markdown-body">)"
           +
           html
           +
           R"(</article></body></html>)";
    m_textPreview->setHtml(allHtml, QUrl("file://" + QFileInfo(path).absolutePath() + '/'));
    htmlFile.write(allHtml.toUtf8());
    htmlFile.close();
    // auto url = QString("file://%1/%2/").arg(workshopPath()).arg(m_curNote.strId());
    // m_textPreview->setHtml(html, QUrl(url));
//    QString owner = Settings::instance()->usernameEn;
//    m_esApi->putNote(owner, html, m_curNote);
//    uploadNoteAttachment(m_curNote);
#endif
}


void Widget::saveMdText() {
    auto notePath = currentNotePath();
    if (notePath.isEmpty()) {
        return;
    }
    QFile file(notePath);
    file.open(QIODevice::WriteOnly);
    const QString &mdText = m_textEdit->toPlainText();
    file.write(mdText.toUtf8());
    file.close();
    updateIndex(mdText, m_curNote.id());
    updateStatistics();
}

void Widget::on_action_newNote() {
    bool ok;
    QString noteName = QInputDialog::getText(this, tr("New Note"),tr("Note name"), QLineEdit::Normal,"untitled", &ok);
    qDebug() << "new note name:" << noteName;
    if (!ok) {
        qDebug() << "user cancel";
        return;
    }
    if (noteName.isEmpty()) {
        qDebug() << "user input is empty";
        return;
    }
    auto item = currentTreeItem();
    if (!item) {
        qDebug() << "current item is nullptr";
        showErrorDialog(tr("current item is null"));
        return;
    }
    int pathId = item->pathId();
    auto strId = Utils::generateId();
    Note note(strId, noteName, pathId);
    QString newNotePath = noteRealPath(note);
    QDir dir;
    dir.mkpath(QFileInfo(newNotePath).path());
    QFile file = QFile(newNotePath);

    qDebug() << "create new note:" << noteName << "in" << newNotePath;
    ok = file.open(QIODevice::WriteOnly);
    if (!ok) {
        qDebug() << "open file fail:" << newNotePath;
        return;
    }
    m_dbManager->addNewNote(note);
    TreeItem* parentItem = item->isFile() ? item->parentItem() : item;
    auto noteItem = new NoteItem(note, parentItem);
    item->setPath(newNotePath);
    auto newNoteIndex = m_treeModel->addNewNode(m_treeView->currentIndex(), noteItem);
    m_treeView->setCurrentIndex(newNoteIndex);
    QString newNoteText = "# " + noteName;
    file.write(newNoteText.toUtf8());
    file.close();
    loadNote(note);
    m_textEdit->setFocus();
}

void Widget::on_action_newFolder() {

    bool ok;
    QString folderName = QInputDialog::getText(this, tr("New Folder"), tr("Folder name"),
                                               QLineEdit::Normal, "folder", &ok);
    qDebug() << "new folder name:" << folderName;
    if (!ok) {
        qDebug() << "user cancel";
        return;
    }
    if (folderName.isEmpty()) {
        qDebug() << "user input is empty";
        return;
    }
    auto item = currentTreeItem();
    if (!item) {
        qDebug() << "current item is nullptr";
        showErrorDialog(tr("current item is null"));
        return;
    }
    auto ret = m_dbManager->isPathExist(folderName, item->pathId());
    if (ret) {
        qDebug() << "path exist." << folderName;
        showErrorDialog(tr("folder exist."));
        return;
    }
    Path path(folderName, item->pathId());
    ret = m_dbManager->addNewPath(path);
    if (!ret) {
        qDebug() << "save to db fail";
        showErrorDialog(tr("new folder: save to db fail"));
        return;
    }
    auto newPathItem = new FolderItem(path, item);
    auto newNoteIndex = m_treeModel->addNewFolder(m_treeView->currentIndex(), newPathItem);
    m_treeView->setCurrentIndex(newNoteIndex);
}
void Widget::loadMdText() {
    auto notePath = currentNotePath();
    loadMdText(notePath);
}
void Widget::loadMdText(const QString &notePath) {

    QFile file(notePath);
    if(!file.exists()) {
        qDebug() << notePath << "is not exist.";
        QMessageBox::critical(this, tr("File"), tr("File not exist."));
        return;
    }
    bool ret = file.open(QIODevice::ReadOnly);
    if (!ret) {
        qDebug() << notePath << "open fail.";
        QMessageBox::critical(this, tr("File"), tr("File open fail."));
        return;
    }
    QString mdText = file.readAll();
    m_textEdit->setText(mdText);
    file.close();
    updateStatistics();
}

void Widget::on_action_trashNote() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    auto ret = m_dbManager->removeNote(item->note().id());
    if (!ret) {
        qDebug() << "trash note fail";
        showErrorDialog(tr("trash note fail"));
        return;
    }
    const QString noteOldPath = workshopPath() + item->note().strId();
    QString noteTrashPath = trashPath() + item->note().strId();
    qDebug() << "trash" << noteOldPath << "to" << noteTrashPath;
    ret = QDir().rename(noteOldPath, noteTrashPath);
    if (!ret) {
        qDebug() << "trash" << noteOldPath << "fail";
        showErrorDialog(tr("trash note fail"));
        return;
    }
    m_treeModel->removeNode(index);
}

void Widget::on_action_trashFolder() {

    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<FolderItem *>(index.internalPointer());
    auto ret = m_dbManager->removePath(item->path().id());
    if (!ret) {
        qDebug() << "trash path fail";
        showErrorDialog(tr("trash folder fail"));
        return;
    }
    m_treeModel->removeNode(index);
    qDebug() << "trash" << item->path().name();
}

TreeItem *Widget::currentTreeItem() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) return nullptr;
    return static_cast<TreeItem*>(index.internalPointer());
}

void Widget::showErrorDialog(const QString &msg) {
    QMessageBox::critical(this, tr("ERROR"), msg);
}

void Widget::updateIndex(QString text, int id) {
    auto f = [this](QString text, int id) {
//        if (!m_jieba) initJieba();
        std::vector<std::string> words;
        std::string s = text.toStdString();
//        m_jieba->Cut(s, words);
        std::unordered_set<std::string> wordSet(words.begin(), words.end());
        QStringList wordList;
        for(const auto& word: wordSet) {
            wordList << QString::fromStdString(word);
        }
//        DbManager db(m_notesPath);
        m_dbManager->updateIndex(wordList, id);
        qDebug() << "update index for note" << id << "finish";
    };
    qDebug() << "update index for note" << id << "start";
    auto ret = QtConcurrent::run(f, text, id);
    Q_UNUSED(ret)
//    f(text, id);
}

void Widget::initSearchDialog() {
    m_searchDialog = new SearchDialog(this);
    // 强制计算搜索框的实际大小
    m_searchDialog->show();
    m_searchDialog->hide();
    connect(m_searchDialog, &SearchDialog::searchTextChanged, this, &Widget::on_searchDialog_searchTextChanged);
    connect(m_searchDialog, &SearchDialog::clickNote, [this](int noteId) {
        auto note = m_dbManager->getNote(noteId);
        if (note.id() == -1) {
            qWarning() << "invalid note id" << noteId;
            return;
        }
        this->loadNote(note);
        m_searchDialog->hide();
    });
}

void Widget::on_searchDialog_searchTextChanged(const QString &text) {
#if 1
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
#endif
#if 0
    auto f = [this](const QString& text) -> QList<Note> {
        std::vector<std::string> words;
        /*
        if (!m_jieba) initJieba();
        m_jieba->Cut(text.toStdString(), words);
         */
        QStringList wordList;
        for(const auto& word: words) {
            wordList << QString::fromStdString(word);
        }
        qDebug() << wordList;
        QList<Note> noteList = m_dbManager->getNoteList(wordList);
        qDebug() << noteList.size();
        return noteList;
    };
    auto callback = [this](QList<Note> noteList) {
        auto model = new QStandardItemModel(this);
        for(const auto& note: noteList) {
            auto item = new QStandardItem(note.title());
            item->setData(QVariant::fromValue(note), Qt::UserRole+1);
            model->appendRow(item);
        }
//    m_listModel->reset(noteList);
        searchResultView()->setModel(model);
        searchResultView()->show();
    };
    auto ret = QtConcurrent::run(f, text);
    Utils::checkFuture<QList<Note>>(ret, callback);
#endif
}

void Widget::initJieba() {
    // TODO: change the path
    const char* const DICT_PATH = "/Users/pikachu/QtProjects/cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "/Users/pikachu/QtProjects/cppjieba/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "/Users/pikachu/QtProjects/cppjieba/dict/user.dict.utf8";
    const char* const IDF_PATH = "/Users/pikachu/QtProjects/cppjieba/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "/Users/pikachu/QtProjects/cppjieba/dict/stop_words.utf8";
    /*
    m_jieba = new cppjieba::Jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);
                          */
}

void Widget::initIndexer()
{
    auto readNoteContent = [this](const Note& note) -> QString {
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

QListView* Widget::searchResultView() {
    if (!m_listView) {
        m_listView = new ListView();
        m_listView->setModel(m_listModel);
    }
    auto x = this->geometry().left() + this->geometry().width() / 2 - m_searchDialog->width() / 2;
    int y = this->geometry().top() + Constant::marginToTop + Constant::searchDialogHeight + 10;
    m_listView->move(x, y);
    connect(m_listView, &ListView::pressed, this, &Widget::on_listView_pressed);
    return m_listView;
}

void Widget::on_listView_pressed(const QModelIndex &index) {
    if (!index.isValid()) {
        return;
    }
    index.data(Qt::UserRole+1);
    auto note = index.data(Qt::UserRole+1).value<Note>();
    loadNote(note);
    m_listView->hide();
    m_searchDialog->hide();
}

void Widget::loadNote(const Note &note) {
    qDebug() << "load" << note.strId() << note.title();
    m_curNote = note;
    const QString &path = noteRealPath(note);
    loadNote(path);
    if (m_showOpenInTyporaTip) {
        Toast::showTip("Press E Open in Typora", this);
        m_showOpenInTyporaTip = false;
    }
}

void Widget::loadNote(const QString &path)
{
    if (!QFile(path).exists()) {
        qWarning() << "note not exist." << path;
    }
    m_fileSystemWatcher->addPath(path);
    m_curNotePath = path;
    qDebug() << "load" << path;
    loadMdText(path);
    updatePreview(path);
    Settings::instance()->lastOpenNotePath = path;
}


void Widget::updateStatistics() {
    /*
    QString mdText = m_textEdit->toPlainText();
    m_wordCountLabel->setText(tr("Statistics: %1 words, %2 characters").arg("...").arg(mdText.size()));
    auto f = [this](const QString& mdText) -> int {
        std::vector<std::string> words;
        jieba()->Cut(mdText.toStdString(), words);
        return words.size();
    };
    QFuture<int> ret = QtConcurrent::run(f, mdText);
    auto callback = [this](int wordCount) {
        m_wordCountLabel->setText(tr("Statistics: %1 words, %2 characters")
        .arg(wordCount).arg(m_textEdit->toPlainText().size()));
    };
    Utils::checkFuture<int>(ret, callback);
     */
}
/*
Jieba *Widget::jieba() {
    if (!m_jieba) initJieba();
    return m_jieba;
}
*/
void Widget::openInTypora(const QString& notePath) {
    qDebug() << "open in typora:" << notePath;
#ifdef Q_OS_WIN
    QStringList pathList;
    QString typoraPath = Settings::instance()->typoraPath;
    pathList << typoraPath;
    pathList << "C:\\Program Files\\Typora\\Typora.exe";
    pathList << "C:\\Program Files (x86)\\Typora\\Typora.exe";
    pathList << "D:\\typora\\Typora\\Typora.exe";
    QString exePath;
    bool exeFind = false;
    for(auto path: pathList) {
        if (QFile(path).exists()) {
            exeFind = true;
            exePath = path;
        }
    }
    if (!exeFind) {
        showErrorDialog("Please install Typora first.");
    } else {
        QStringList cmd;
        cmd << notePath;
        QProcess::startDetached(exePath,cmd);
    }
#else
    QStringList cmd;
    cmd << "-a" << "typora" << notePath;
    QProcess::startDetached("open",cmd);
#endif
}

void Widget::openNoteInTypora(const Note& note) {
    openInTypora(noteRealPath(note));
}
void Widget::on_action_openInTypora() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!item) {
        qDebug() << "open in typora fail, NPE";
        showErrorDialog(tr("open in typora fail"));
        return;
    }
    const QString notePath = noteRealPath(item->note());
    openInTypora(notePath);
}

QString Widget::currentNotePath() {
    return m_curNotePath;
}

void Widget::on_action_exportNoteToHTML() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!item) {
        qDebug() << "export to HTML fail, NPE";
        showErrorDialog(tr("export to HTML fail"));
        return;
    }
    auto dirName = QFileDialog::getSaveFileName(this, tr("Export Note to HTML"));
    m_htmlExporter->exportToHtml(item->note(), dirName);
}

QString Widget::noteRealPath(const Note& note) {
    return workshopPath() + note.strId() + "/index.md";
}


void Widget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    qDebug() << "resize" << this->geometry();
    Settings::instance()->mainWindowGeometry = this->geometry();
}

void Widget::addNoteTo() {
    auto item = currentTreeItem();
    if (!item) {
        qDebug() << "item is nullptr";
        return;
    }
    qDebug() << "add note to";
//    ChooseFolderWidget w;

}

void Widget::initSystemTrayIcon()
{
#ifdef Q_OS_MAC
    m_systemTrayIcon->setIcon(QIcon(QPixmap(":/icon/tray_128x128.png")));
#else
    m_systemTrayIcon->setIcon(QIcon(QPixmap(":/icon/notebook_128x128.png")));
#endif
    auto menu = new QMenu();
    menu->addAction(tr("Show"), [this](){
       this->showNormal();
    });

    menu->addAction(tr("Settings"), [this](){
        SettingsDialog dialog;
        connect(&dialog, &SettingsDialog::requestReindex, [this](){
            this->initIndexer();
        });
        dialog.exec();
    });
#ifdef ENABLE_TROJAN
    auto a = new QAction("Trojan");
    a->setCheckable(true);
    menu->addAction(a);
    connect(a, &QAction::triggered, [this, a](){
        qDebug() << "trojan triggered" << a->isChecked();
        if (!m_trojanThread) {
            m_trojanThread = new TrojanThread();
            connect(m_trojanThread, &TrojanThread::started, this, [this, a]() {
                int localPort = m_trojanThread->localPort();
                a->setText(tr("Trojan: %1").arg(localPort));
            }, Qt::QueuedConnection);
            connect(m_trojanThread, &TrojanThread::stopped, this, [this, a]() {
                a->setText(tr("Trojan"));
            }, Qt::QueuedConnection);
        }
        if (a->isChecked()) {
            QString configPath = Settings::instance()->trojanConfigPath;
            bool ok = m_trojanThread->loadConfig(configPath);
            if (ok) {
                m_trojanThread->start();
            } else {
                showWarning(tr("Start Trojan"),
                            tr(R"(Trojan config is error.
Please check your config file.
Current config: %1)").arg(configPath));
                a->setChecked(false);
            }

        } else {
            m_trojanThread->stop();
        }
    });
#endif
    menu->addAction(tr("Show Config"), [this]() {
        Settings::instance();
        auto path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).first();
        auto dir = QDir(path);
        dir.cdUp();
        QString url = QString("%1/MyNotes.ini").arg(dir.absolutePath());
        QDesktopServices::openUrl(QUrl(url));
    });
    menu->addAction(tr("About"), [this](){
        AboutDialog dialog(this);
        dialog.exec();
    });
    menu->addAction(tr("About Qt"), [this](){
        qApp->aboutQt();
    });
    menu->addAction(tr("Quit"), [this](){
        qApp->quit();
    });
    m_systemTrayIcon->setContextMenu(menu);
    m_systemTrayIcon->show();
    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        qDebug() << reason;
        switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this->showNormal();
            break;
        default:
            break;
        }
    });
}

void Widget::on_fileSystemWatcher_fileChanged(const QString &path) {
    qDebug () << "file change:" << path;
    // 如果当前变更的文档在tab页中，更新tab页的内容
    for(auto tab: m_tabWidget->tabs()) {
        if (tab->filePath() == path) {
            tab->reload();
        }
    }
    if (path.startsWith(workshopPath())) {
        if (path.endsWith("index.md")) {
            // 标准的笔记处理
            QString noteStrId = getWorkshopNoteStrIdFromPath(path);
            qDebug() << "note id:" << noteStrId;
            auto note = m_dbManager->getNote(noteStrId);
            if (note.id() == -1) {
                qWarning() << "invalid note from strId:" << noteStrId;
                showErrorDialog(tr("invalid note from strId: %1").arg(noteStrId));
            } else {
                updatePreview();
                if (Settings::instance()->syncWorkshopAuto) {
                    m_syncService->syncWorkshopFile(note);
                }
            }
        } else {

        }
    } else {
        if (Settings::instance()->syncWatchingAuto) {
            m_syncService->syncWatchingFile(path);
        }
    }
}

void Widget::initFileSystemWatcher() {
    connect(m_fileSystemWatcher, &FileSystemWatcher::fileChanged,
            this, &Widget::on_fileSystemWatcher_fileChanged);
    connect(m_fileSystemWatcher, &FileSystemWatcher::renameFolder, [this](const QString& oldFolderPath, const QString& newFolderPath){
        qDebug() << "rename folder:" << oldFolderPath << newFolderPath;
        m_treeModel->updateWatchingDir(oldFolderPath, newFolderPath);
        m_syncService->updateProfile();
    });
    connect(m_fileSystemWatcher, &FileSystemWatcher::newFolder, [this](const QString& newFolderPath){
        m_treeModel->addWatchingNode(newFolderPath);
        m_syncService->updateProfile();
    });
    connect(m_fileSystemWatcher, &FileSystemWatcher::newFile, [this](const QString& newFilePath){
        QStringList syncSuffix = Utils::syncSuffix();
        for(const auto& suffix: syncSuffix) {
            if (newFilePath.endsWith(suffix)) {
                m_treeModel->addWatchingNode(newFilePath);
                m_syncService->syncWatchingFile(newFilePath);
                m_syncService->updateProfile();
                break;
            }
        }
    });
    connect(m_fileSystemWatcher, &FileSystemWatcher::deleteFolder, [this](const QString& path){
        m_treeModel->removeWatchingNote(path);
        m_syncService->updateProfile();
    });
    connect(m_fileSystemWatcher, &FileSystemWatcher::deleteFile, [this](const QString& path){
        m_treeModel->removeWatchingNote(path);
        m_syncService->updateProfile();
    });
}

// 设置开机自启动
// http://blog.sina.com.cn/s/blog_a6fb6cc90101feia.html
void Widget::setAutoStart() {
#ifdef Q_OS_WIN
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
    QString applicationName = QApplication::applicationName();
    auto settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    QString applicationPath = QApplication::applicationFilePath();
    settings->setValue(applicationName, '"' + applicationPath.replace("/", "\\") + '"');
    delete settings;
#endif
#ifdef Q_OS_MAC
    // mac开机自启动
    // https://gist.github.com/andreybutov/33783bca1af9db8f9f36c463c77d7a86
    auto macOSXAppBundlePath = []() {
        QDir dir = QDir (QCoreApplication::applicationDirPath() );
        dir.cdUp();
        dir.cdUp();
        QString absolutePath = dir.absolutePath();
        // absolutePath will contain a "/" at the end,
        // but we want the clean path to the .app bundle
        if ( absolutePath.length() > 0 && absolutePath.right(1) == "/" ) {
            absolutePath.chop(1);
        }
        return absolutePath;
    };
    QStringList args;
    args << QString("-e tell application \"System Events\" to make login item at end ") +
            "with properties {path:\"" + macOSXAppBundlePath() + "\", hidden:false}";

    QProcess::execute("osascript", args);
#endif
}

void Widget::initShortcut() {
#if 0
    // 用一种比较奇怪的方式，兼容焦点在webengine时没法按E进入Typora编辑的问题
    auto editShortcut = new QShortcut((Qt::Key_E), m_textPreview);
    QObject::connect(editShortcut, &QShortcut::activated, m_textPreview, [this]() {
        qDebug() << "from web engine";
        openInTypora(currentNotePath());
    });
    auto searchShortcut = new QShortcut((Qt::Key_F), m_textPreview);
    connect(searchShortcut, &QShortcut::activated, m_textPreview, [this]() {
        QString ip = Settings::instance()->serverIp;
        QString url = "http://"+ip;
        qDebug() << "search" << url;
        m_textPreview->setUrl(QUrl(url));
    });
    auto openInBrowserShortcut = new QShortcut((Qt::Key_B), m_textPreview);
    connect(openInBrowserShortcut, &QShortcut::activated, m_textPreview, [this]() {
        QString ip = Settings::instance()->serverIp;
        QString owner = Settings::instance()->usernameEn;
        const QString &url = QString("http://%1/%2/%3/")
                .arg(ip).arg(owner).arg(currentNoteStrId());
        qDebug() << "open in browser" << url;
        QDesktopServices::openUrl(QUrl(url));
    });
#endif
}

QString Widget::currentNoteStrId() {
    QString ret;
    auto path = currentNotePath();
    if (path.startsWith(workshopPath())) {
        return getWorkshopNoteStrIdFromPath(path);
    } else {
        return Utils::md5(path);
    }
}

QString Widget::getWorkshopNoteStrIdFromPath(const QString& path) {
    QStringList segs = path.split('/');
    QString noteStrId = segs[segs.size() - 2];
    return noteStrId;
}

void Widget::showNextTab() {
    qDebug() << "next tab";
    int newTabIndex = (m_tabWidget->currentIndex() + 1) % m_tabWidget->count();
    m_tabWidget->setCurrentWidget(m_tabWidget->tabAt(newTabIndex));
}


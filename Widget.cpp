#include "Widget.h"
#include "NoteFileService.h"
#include "SyncService.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "TreeView.h"
#include "DbManager.h"
#include "TrayIconManager.h"
#include "NoteEditorWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFile>
#include <QMenu>
#include <QEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QUrlQuery>
#include <QStandardPaths>
#include <QIcon>
#include <QPixmap>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include "utils/Utils.h"
#include "SearchController.h"
#include "utils/Constant.h"
#include "ElasticSearchRestApi.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include "Http.h"
#include <QDesktopServices>
#include <QStringList>
#include "FileSystemWatcher.h"
#include "AboutDialog.h"
#include "Indexer.h"
#include "HtmlExporter.h"
#ifdef ENABLE_TROJAN
#include "TrojanThread.h"
#endif
Widget::Widget(QWidget *parent)
        : PiWidget(parent),
        m_settings(Settings::instance())
        , m_fileSystemWatcher(FileSystemWatcher::instance())
        , m_timer(new QTimer(this))
//        , m_indexer(new Indexer())
#ifdef ENABLE_TROJAN
        , m_trojanThread(nullptr)
#endif
//        ,m_jieba(nullptr)
        {
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
    // 处理Ctrl+S保存
    m_treeView->installEventFilter(this);
    m_treeView->setMinimumWidth(300);
//    initShortcut();
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
    m_noteFileService = new NoteFileService(m_dbManager, workshopPath(), trashPath(), this);
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
    m_searchController = new SearchController(m_dbManager, m_indexer, m_notesPath, this);
    connect(m_searchController, &SearchController::noteSelected, this, [this](int noteId) {
        auto note = m_dbManager->getNote(noteId);
        if (note.id() != -1) {
            m_noteEditorWidget->loadNote(note);
        }
    });
    m_noteEditorWidget = new NoteEditorWidget();
    m_noteEditorWidget->setDependencies(m_indexer, m_noteFileService, m_fileSystemWatcher, m_settings, m_dbManager, workshopPath());
    auto mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(m_treeView);
    splitter->addWidget(m_noteEditorWidget);
    splitter->setStretchFactor(1, 3);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
    setWindowIcon(QIcon(QPixmap(":/icon/notebook_128x128.png")));
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
    m_trayIconManager = new TrayIconManager(m_settings, this);
    m_trayIconManager->setup(this);
    connect(m_trayIconManager, &TrayIconManager::showRequested, this, &QWidget::showNormal);
    connect(m_trayIconManager, &TrayIconManager::quitRequested, qApp, &QApplication::quit);
    connect(m_trayIconManager, &TrayIconManager::settingsRequested, this, [this]() {
        SettingsDialog dialog;
        connect(&dialog, &SettingsDialog::requestReindex, m_searchController, &SearchController::initIndexer);
        dialog.exec();
    });
    connect(m_trayIconManager, &TrayIconManager::aboutRequested, this, [this]() {
        AboutDialog dialog(this);
        dialog.exec();
    });
    connect(m_trayIconManager, &TrayIconManager::syncAllRequested, m_syncService, &SyncService::syncAll);
    // 读最后一次打开的笔记
    m_noteEditorWidget->loadLastOpenedNote();
}

Widget::~Widget() {
}

void Widget::on_treeView_pressed(const QModelIndex &index) {
    if (!index.isValid()) return;
    auto item = static_cast<TreeItem *>(index.internalPointer());
    auto tryLoadNote = [this](const QString& notePath) {
        qDebug() << "note path: " << notePath;
        // 右键选中笔记时，如果当前笔记就是选中的笔记，不重新载入笔记内容
        if (notePath == m_noteEditorWidget->currentFilePath()) {
            return;
        }
        if (!notePath.isEmpty()) {
            m_noteEditorWidget->loadNote(notePath);
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
        notePath = m_noteFileService->noteRealPath(note);
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
                qDebug() << "add note to";
            });
            auto noteItem = (NoteItem*)item;
            auto note = noteItem->note();
            auto notePath = m_noteFileService->noteRealPath(note);
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
                        QString url = this->m_noteFileService->noteRealPath(note)+"/..";
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
            m_noteEditorWidget->save();
            m_noteEditorWidget->refreshPreview();
            return true;
        }
        if (event->key() == Qt::Key_E) {
            m_noteEditorWidget->openInTypora(m_noteEditorWidget->currentFilePath());
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
                m_searchController->showSearchDialog(this->geometry());
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
    hide();
    event->ignore();
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

void Widget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    qDebug() << "resize" << this->geometry();
    Settings::instance()->mainWindowGeometry = this->geometry();
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
    auto note = m_noteFileService->createNote(noteName, pathId);
    if (note.id() == -1) {
        qDebug() << "create note fail";
        return;
    }
    QString newNotePath = m_noteFileService->noteRealPath(note);
    TreeItem* parentItem = item->isFile() ? item->parentItem() : item;
    auto noteItem = new NoteItem(note, parentItem);
    item->setPath(newNotePath);
    auto newNoteIndex = m_treeModel->addNewNode(m_treeView->currentIndex(), noteItem);
    m_treeView->setCurrentIndex(newNoteIndex);
    m_noteEditorWidget->loadNote(note);
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
    if (m_noteFileService->isPathExist(folderName, item->pathId())) {
        qDebug() << "path exist." << folderName;
        showErrorDialog(tr("folder exist."));
        return;
    }
    auto path = m_noteFileService->createFolder(folderName, item->pathId());
    auto newPathItem = new FolderItem(path, item);
    auto newNoteIndex = m_treeModel->addNewFolder(m_treeView->currentIndex(), newPathItem);
    m_treeView->setCurrentIndex(newNoteIndex);
}

void Widget::on_action_trashNote() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!m_noteFileService->trashNote(item->note())) {
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
    if (!m_noteFileService->trashFolder(item->path())) {
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

void Widget::on_action_openInTypora() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) return;
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!item) { showErrorDialog(tr("open in typora fail")); return; }
    m_noteEditorWidget->openInTypora(m_noteFileService->noteRealPath(item->note()));
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

void Widget::on_fileSystemWatcher_fileChanged(const QString &path) {
    qDebug () << "file change:" << path;
    // 如果当前变更的文档在tab页中，更新tab页的内容
    m_noteEditorWidget->reloadTabAt(path);
    if (path.startsWith(workshopPath())) {
        if (path.endsWith("index.md")) {
            // 标准的笔记处理
            QString noteStrId = m_noteFileService->noteStrIdFromWorkshopPath(path);
            qDebug() << "note id:" << noteStrId;
            auto note = m_dbManager->getNote(noteStrId);
            if (note.id() == -1) {
                qWarning() << "invalid note from strId:" << noteStrId;
                showErrorDialog(tr("invalid note from strId: %1").arg(noteStrId));
            } else {
                m_noteEditorWidget->refreshPreview();
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

void Widget::showNextTab() {
    m_noteEditorWidget->showNextTab();
}

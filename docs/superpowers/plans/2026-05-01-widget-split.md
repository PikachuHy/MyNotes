# Widget 拆分 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Split Widget (1818-line God Object) into 7 single-responsibility classes, making the editing/preview pipeline independently modifiable for future syntax highlighting and real-time preview features.

**Architecture:** Extract 6 new classes from Widget in dependency order. Widget becomes a ~200-line shell that owns and wires all components together. Each extraction moves methods verbatim with minimal changes — renaming for consistency but preserving logic.

**Tech Stack:** Qt 5/6 (C++17), QWidgets, CMake

**Verification after each task:** `cmake --build build` must pass. Run `./build/MyNotes` and confirm basic note operations (create, open, save) still work.

---

### Task 1: Create WatchingFileHtmlVisitor and HtmlExporter

**Files:**
- Create: `WatchingFileHtmlVisitor.h`
- Create: `WatchingFileHtmlVisitor.cpp`
- Create: `HtmlExporter.h`
- Create: `HtmlExporter.cpp`
- Modify: `Widget.h` — remove `generateHTML` methods
- Modify: `Widget.cpp` — remove code moved to new files, add `#include "HtmlExporter.h"`, use it
- Modify: `CMakeLists.txt` — add new source files to libMyNotes

- [ ] **Step 1: Create WatchingFileHtmlVisitor.h**

```cpp
#ifndef MYNOTES_WATCHINGFILEHTMLVISITOR_H
#define MYNOTES_WATCHINGFILEHTMLVISITOR_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>
#include "QtMarkdownParser"

class WatchingFileHtmlVisitor : public MultipleVisitor<Header,
        Text, ItalicText, BoldText, ItalicBoldText,
        Image, Link, CodeBlock, InlineCode, Paragraph,
        UnorderedList, OrderedList,
        Hr, QuoteBlock, Table> {
public:
    WatchingFileHtmlVisitor(const QString& path);

    void visit(Header *node) override;
    void visit(Text *node) override;
    void visit(ItalicText *node) override;
    void visit(BoldText *node) override;
    void visit(ItalicBoldText *node) override;
    void visit(Image *node) override;
    void visit(Link *node) override;
    void visit(CodeBlock *node) override;
    void visit(InlineCode *node) override;
    void visit(Paragraph *node) override;
    void visit(UnorderedList *node) override;
    void visit(OrderedList *node) override;
    void visit(Hr *node) override;
    void visit(QuoteBlock *node) override;
    void visit(Table *node) override;

    String html() { return m_html; }
    QStringList pathList() { return m_pathList; }

private:
    String m_html;
    QStringList m_pathList;
    QString m_notePath;
    QString m_noteDir;
};

#endif
```

- [ ] **Step 2: Create WatchingFileHtmlVisitor.cpp**

Copy the `WatchingFileHtmlVisitor` implementation from `Widget.cpp` lines 1353-1507. This is the `struct WatchingFileHtmlVisitor: MultipleVisitor<...> { ... };` definition and all its `visit()` method bodies. Paste into the new file verbatim, changing only the header include and the `struct` keyword to match the header (the header already declares it as `class`; the method bodies are identical regardless).

The content to extract starts at line 1353 with the struct definition and ends at line 1507 with `};`.

- [ ] **Step 3: Create HtmlExporter.h**

```cpp
#ifndef MYNOTES_HTMLEXPORTER_H
#define MYNOTES_HTMLEXPORTER_H

#include <QObject>
#include <QString>
#include "DbModel.h"

class HtmlExporter : public QObject {
    Q_OBJECT
public:
    explicit HtmlExporter(const QString& workshopPath, QObject *parent = nullptr);

    QString generateHtml(const Note& note);
    QString generateHtml(const QString& mdPath, const QString& title);
    void exportToHtml(const Note& note, const QString& dirPath);
    static QString wrapWithTemplate(const QString& title, const QString& bodyHtml);

private:
    QString noteRealPath(const Note& note) const;
    QString m_workshopPath;
};

#endif
```

- [ ] **Step 4: Create HtmlExporter.cpp**

Move the three `Widget::generateHTML(...)` methods (lines 1061-1101) and `Widget::on_action_exportNoteToHTML()` (lines 1012-1046) logic into this file. Also extract the duplicate HTML template wrapping logic from `Widget::uploadNote()` (lines 1650-1668) into `wrapWithTemplate`.

```cpp
#include "HtmlExporter.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include "QtMarkdownParser"

HtmlExporter::HtmlExporter(const QString& workshopPath, QObject *parent)
    : QObject(parent), m_workshopPath(workshopPath)
{
}

QString HtmlExporter::noteRealPath(const Note& note) const
{
    return m_workshopPath + note.strId() + "/index.md";
}

QString HtmlExporter::generateHtml(const Note& note)
{
    return generateHtml(noteRealPath(note), note.title());
}

QString HtmlExporter::generateHtml(const QString& mdPath, const QString& title)
{
    QFile mdFile(mdPath);
    bool ok = mdFile.open(QIODevice::ReadOnly);
    if (!ok) {
        qWarning() << "open fail:" << mdPath;
        return {};
    }
    Document doc(mdFile.readAll());
    mdFile.close();
    auto html = doc.toHtml();
    return wrapWithTemplate(title, html);
}

void HtmlExporter::exportToHtml(const Note& note, const QString& dirPath)
{
    QString notePath = noteRealPath(note);
    QDir noteDir(notePath);
    noteDir.cdUp();
    QFileInfoList fileInfoList = noteDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& fileInfo : fileInfoList) {
        if (fileInfo.fileName() == "index.md") continue;
        QString targetFile = QDir(dirPath).filePath(fileInfo.fileName());
        QFile::copy(fileInfo.filePath(), targetFile);
    }
    QString htmlPath = QDir(dirPath).filePath("index.html");
    QFile htmlFile(htmlPath);
    htmlFile.open(QIODevice::WriteOnly);
    htmlFile.write(generateHtml(notePath, note.title()).toUtf8());
    htmlFile.close();
    QDesktopServices::openUrl(QUrl(QString("file://%1").arg(dirPath)));
}

QString HtmlExporter::wrapWithTemplate(const QString& title, const QString& bodyHtml)
{
    QFile cssFile(":css/css518.css");
    cssFile.open(QIODevice::ReadOnly);
    QString css = cssFile.readAll();
    cssFile.close();
    return R"(<!DOCTYPE html><html><head>
<meta charset="utf-8">
<meta name='viewport' content='width=device-width initial-scale=1'>
<title>)"
        + title
        + R"(</title>
<link href='https://fonts.loli.net/css?family=Open+Sans:400italic,700italic,700,400&subset=latin,latin-ext' rel='stylesheet' type='text/css' />

<style type='text/css'>)"
        + css
        + R"("</style>
</head>
<body class='typora-export'>
<div id='write'  class=''>)"
        + bodyHtml
        + R"(</div></body></html>)";
}
```

- [ ] **Step 5: Update Widget.h — remove generateHTML declarations**

Remove these method declarations from `Widget.h`:
```cpp
QString generateHTML(const Note& note);
QString generateHTML(const QString& path, const QString& title);
void generateHTML(const Note& note, const QString& path);
```

Add a forward declaration and member for HtmlExporter:
```cpp
class HtmlExporter;
// in private section:
HtmlExporter* m_htmlExporter;
```

- [ ] **Step 6: Update Widget.cpp — Wire HtmlExporter**

In Widget constructor, after `m_notesPath` is set:
```cpp
m_htmlExporter = new HtmlExporter(workshopPath(), this);
```

Replace all `generateHTML(...)` calls:
- `this->generateHTML(note)` → `m_htmlExporter->generateHtml(note)`
- `this->generateHTML(noteRealPath(note), note.title())` → `m_htmlExporter->generateHtml(note)`
- `this->generateHTML(path, title)` → `m_htmlExporter->generateHtml(path, title)`

In `on_action_exportNoteToHTML()`, replace the method body with:
```cpp
void Widget::on_action_exportNoteToHTML() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) return;
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!item) {
        showErrorDialog(tr("export to HTML fail"));
        return;
    }
    auto dirName = QFileDialog::getSaveFileName(this, tr("Export Note to HTML"));
    if (dirName.isEmpty()) return;
    if (!QDir().exists(dirName)) {
        QDir().mkdir(dirName);
    }
    m_htmlExporter->exportToHtml(item->note(), dirName);
}
```

Remove the `generateHTML` and `on_action_exportNoteToHTML` implementations (now in HtmlExporter).

Remove `WatchingFileHtmlVisitor` class (lines 1353-1507) — now in its own file.

- [ ] **Step 7: Add to CMakeLists.txt**

Add to `target_sources(libMyNotes PRIVATE ...)`:
```cmake
WatchingFileHtmlVisitor.cpp WatchingFileHtmlVisitor.h
HtmlExporter.cpp HtmlExporter.h
```

- [ ] **Step 8: Build and verify**

```bash
cmake --build build
```

Fix any compilation errors. Run `./build/MyNotes` and verify: export a note to HTML, confirm the output file works in a browser.

---

### Task 2: Extract SyncService

**Files:**
- Create: `SyncService.h`
- Create: `SyncService.cpp`
- Modify: `Widget.h` — remove sync method declarations
- Modify: `Widget.cpp` — replace sync methods with SyncService usage
- Modify: `CMakeLists.txt` — add new files

- [ ] **Step 1: Create SyncService.h**

```cpp
#ifndef MYNOTES_SYNCSERVICE_H
#define MYNOTES_SYNCSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "DbModel.h"

class ElasticSearchRestApi;
class HtmlExporter;
class Settings;
struct ServerNoteInfo;

class SyncService : public QObject {
    Q_OBJECT
public:
    explicit SyncService(ElasticSearchRestApi* esApi, HtmlExporter* htmlExporter, QObject *parent = nullptr);

    void syncAll();
    void syncAllWatching();
    void syncWatchingFolder(const QString& path);
    void syncWatchingFile(const QString& path);
    void syncWorkshopFile(const Note& note);

    void uploadNoteAttachment(const Note& note);
    void uploadFile(const QString& noteStrId, const QString& filePath);
    void uploadNote(const ServerNoteInfo& info);

    void updateProfile();

    // Signals for UI feedback
    void showSyncResult(const QString& msg);

private:
    void doUpdateProfile();
    void traversalFileTree(const QString& path, QStringList& pathList);
    static QByteArray fileChecksum(const QString& fileName, QCryptographicHash::Algorithm hashAlgorithm);

    ElasticSearchRestApi* m_esApi;
    HtmlExporter* m_htmlExporter;
    Settings* m_settings;
};

#endif
```

- [ ] **Step 2: Create SyncService.cpp**

Move these methods from Widget.cpp verbatim into SyncService.cpp:

| From Widget.cpp | To SyncService.cpp |
|---|---|
| `fileChecksum()` (line 60-71) | `fileChecksum()` (static) |
| `uploadNoteAttachment()` (line 1120-1160) | `uploadNoteAttachment()` |
| `syncAll()` (line 1248-1257) | `syncAll()` |
| `uploadNote()` (line 1643-1680) | `uploadNote()` — replace inline HTML wrapping with `m_htmlExporter->wrapWithTemplate(info.title, info.noteHtml)` |
| `syncWatchingFile()` (line 1508-1561) | `syncWatchingFile()` — replace inline `WatchingFileHtmlVisitor` usage with `m_htmlExporter->wrapWithTemplate(title, visitor.html())` |
| `syncWatchingFolder()` (line 1333-1352) | `syncWatchingFolder()` |
| `syncWorkshopFile()` (line 1682-1704) | `syncWorkshopFile()` |
| `syncAllWatching()` (line 1325-1331) | `syncAllWatching()` |
| `showSyncResult()` (line 1564-1568) | `showSyncResult()` |
| `uploadFile()` (line 1605-1641) | `uploadFile()` |
| `updateProfile()` / `_updateProfile()` (line 1749-1793) | `updateProfile()` / `doUpdateProfile()` |
| `traversalFileTree()` (line 1795-1817) | `traversalFileTree()` |

In the constructor, store `m_esApi`, `m_htmlExporter` dependencies. Replace `Settings::instance()` calls with `m_settings` (initialized in constructor).

Replace inline HTML template code in `uploadNote()` and `syncWorkshopFile()` with `m_htmlExporter->wrapWithTemplate(title, bodyHtml)`.

- [ ] **Step 3: Update Widget.h**

Remove these method declarations:
```cpp
void syncAll();
void syncAllWatching();
void syncWatchingFile(const QString& path);
void syncWatchingFolder(const QString& path);
void showSyncResult(const QString& msg);
void uploadFile(const QString& noteStrId, const QString& path);
void uploadNote(const ServerNoteInfo& info);
void syncWorkshopFile(const Note& note);
void uploadNoteAttachment(const Note& note);
void updateProfile();
void _updateProfile();
void traversalFileTree(const QString& path, QStringList& pathList);
```

Replace `ElasticSearchRestApi* m_esApi;` with `SyncService* m_syncService;` (and remove `#include "ElasticSearchRestApi.h"`).

- [ ] **Step 4: Update Widget.cpp — Wire SyncService**

In the constructor, after creating `m_htmlExporter`:
```cpp
m_syncService = new SyncService(m_esApi, m_htmlExporter, this);
```

Replace method calls:
- `this->syncAll()` → `m_syncService->syncAll()`
- `this->syncAllWatching()` → `m_syncService->syncAllWatching()`
- `this->syncWatchingFile(path)` → `m_syncService->syncWatchingFile(path)`
- `this->syncWatchingFolder(path)` → `m_syncService->syncWatchingFolder(path)`
- `this->syncWorkshopFile(note)` → `m_syncService->syncWorkshopFile(note)`
- `this->uploadFile(noteId, path)` → `m_syncService->uploadFile(noteId, path)`
- `this->uploadNote(info)` → `m_syncService->uploadNote(info)`
- `this->uploadNoteAttachment(note)` → `m_syncService->uploadNoteAttachment(note)`
- `this->updateProfile()` → `m_syncService->updateProfile()`
- `showSyncResult(msg)` → `m_syncService->showSyncResult(msg)`

Remove all the moved method implementations.

In `on_treeView_customContextMenuRequested`, update sync-related menu action connections to call `m_syncService` methods directly.

- [ ] **Step 5: Update CMakeLists.txt**

Add:
```cmake
SyncService.cpp SyncService.h
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build build
```

Run and test: right-click → Sync Folder / Sync Note / Sync All, confirm no crashes.

---

### Task 3: Extract NoteFileService

**Files:**
- Create: `NoteFileService.h`
- Create: `NoteFileService.cpp`
- Modify: `Widget.h`
- Modify: `Widget.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create NoteFileService.h**

```cpp
#ifndef MYNOTES_NOTEFILESERVICE_H
#define MYNOTES_NOTEFILESERVICE_H

#include <QObject>
#include <QString>
#include "DbModel.h"

class DbManager;

class NoteFileService : public QObject {
    Q_OBJECT
public:
    explicit NoteFileService(DbManager* dbManager, const QString& workshopPath,
                             const QString& trashPath, QObject *parent = nullptr);

    Note createNote(const QString& name, int parentPathId);
    Path createFolder(const QString& name, int parentId);
    bool trashNote(const Note& note);
    bool trashFolder(const Path& path);

    QString noteRealPath(const Note& note) const;
    Note getNote(int id);
    Note getNote(const QString& strId);
    Path getPath(int id);
    bool isPathExist(const QString& name, int parentId);
    QString noteStrIdFromWorkshopPath(const QString& path) const;

signals:
    void noteTrashed(int noteId);
    void folderTrashed(int folderId);

private:
    DbManager* m_dbManager;
    QString m_workshopPath;
    QString m_trashPath;
};

#endif
```

- [ ] **Step 2: Create NoteFileService.cpp**

Move the core logic (not UI) from these Widget methods:

| From Widget method | Extracted logic |
|---|---|
| `on_action_newNote()` (line 568-611) | Create Note in DB, create file on disk, return Note |
| `on_action_newFolder()` (line 613-648) | Create Path in DB, return Path |
| `on_action_trashNote()` (line 674-696) | Call DbManager::removeNote, move file to trash |
| `on_action_trashFolder()` (line 698-713) | Call DbManager::removePath |
| `noteRealPath()` (line 1056) | One-liner: `m_workshopPath + note.strId() + "/index.md"` |
| `getWorkshopNoteStrIdFromPath()` (line 1743) | Split path by '/', return second-to-last segment |
| `currentNoteStrId()` (line 1733) | Logic for extracting strId from workshop or watched path |

```cpp
#include "NoteFileService.h"
#include "DbManager.h"
#include "Utils.h"
#include <QFile>
#include <QDir>
#include <QDebug>

NoteFileService::NoteFileService(DbManager* dbManager, const QString& workshopPath,
                                 const QString& trashPath, QObject *parent)
    : QObject(parent), m_dbManager(dbManager), m_workshopPath(workshopPath), m_trashPath(trashPath)
{
}

Note NoteFileService::createNote(const QString& name, int parentPathId)
{
    QString strId = Utils::generateId();
    Note note(strId, name, parentPathId);
    QString newNotePath = noteRealPath(note);
    QDir dir;
    dir.mkpath(QFileInfo(newNotePath).path());
    QFile file(newNotePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "open file fail:" << newNotePath;
        return Note();
    }
    m_dbManager->addNewNote(note);
    QString newNoteText = "# " + name;
    file.write(newNoteText.toUtf8());
    file.close();
    return note;
}

Path NoteFileService::createFolder(const QString& name, int parentId)
{
    if (m_dbManager->isPathExist(name, parentId)) {
        qDebug() << "path exist." << name;
        return Path();
    }
    Path path(name, parentId);
    bool ret = m_dbManager->addNewPath(path);
    if (!ret) {
        qDebug() << "save to db fail";
        return Path();
    }
    return path;
}

bool NoteFileService::trashNote(const Note& note)
{
    bool ret = m_dbManager->removeNote(note.id());
    if (!ret) {
        qDebug() << "trash note fail";
        return false;
    }
    const QString noteOldPath = noteRealPath(note);
    QString noteTrashPath = m_trashPath + note.strId();
    qDebug() << "trash" << noteOldPath << "to" << noteTrashPath;
    ret = QDir().rename(noteOldPath, noteTrashPath);
    if (!ret) {
        qDebug() << "trash" << noteOldPath << "fail";
        return false;
    }
    emit noteTrashed(note.id());
    return true;
}

bool NoteFileService::trashFolder(const Path& path)
{
    bool ret = m_dbManager->removePath(path.id());
    if (!ret) {
        qDebug() << "trash path fail";
        return false;
    }
    emit folderTrashed(path.id());
    return true;
}

QString NoteFileService::noteRealPath(const Note& note) const
{
    return m_workshopPath + note.strId() + "/index.md";
}

Note NoteFileService::getNote(int id) { return m_dbManager->getNote(id); }
Note NoteFileService::getNote(const QString& strId) { return m_dbManager->getNote(strId); }
Path NoteFileService::getPath(int id) { return m_dbManager->getPath(id); }
bool NoteFileService::isPathExist(const QString& name, int parentId) { return m_dbManager->isPathExist(name, parentId); }

QString NoteFileService::noteStrIdFromWorkshopPath(const QString& path) const
{
    QStringList segs = path.split('/');
    return segs[segs.size() - 2];
}
```

- [ ] **Step 3: Update Widget.h**

Remove these method declarations:
```cpp
void on_action_newNote();
void on_action_newFolder();
void on_action_trashNote();
void on_action_trashFolder();
void addNoteTo();
Note m_curNote;  // will move to NoteEditorWidget in Task 6
QString noteRealPath(const Note& note);
QString currentNoteStrId();
QString getWorkshopNoteStrIdFromPath(const QString& path);
```

Add:
```cpp
class NoteFileService;
// in private section:
NoteFileService* m_noteFileService;
```

- [ ] **Step 4: Update Widget.cpp — Wire NoteFileService**

In constructor:
```cpp
m_noteFileService = new NoteFileService(m_dbManager, workshopPath(), trashPath(), this);
```

Rewrite `on_action_newNote()` to use NoteFileService:
```cpp
void Widget::on_action_newNote() {
    bool ok;
    QString noteName = QInputDialog::getText(this, tr("New Note"), tr("Note name"),
                                             QLineEdit::Normal, "untitled", &ok);
    if (!ok || noteName.isEmpty()) return;
    auto item = currentTreeItem();
    if (!item) { showErrorDialog(tr("current item is null")); return; }
    int pathId = item->pathId();
    Note note = m_noteFileService->createNote(noteName, pathId);
    if (note.id() == -1) { showErrorDialog(tr("create note fail")); return; }
    TreeItem* parentItem = item->isFile() ? item->parentItem() : item;
    auto noteItem = new NoteItem(note, parentItem);
    auto newNoteIndex = m_treeModel->addNewNode(m_treeView->currentIndex(), noteItem);
    m_treeView->setCurrentIndex(newNoteIndex);
    // Load the note — will use NoteEditorWidget in Task 6, for now call directly
    loadNote(note);
    m_textEdit->setFocus();
}
```

Similarly rewrite `on_action_newFolder()`, `on_action_trashNote()`, `on_action_trashFolder()` to delegate to `m_noteFileService` while keeping the TreeModel/TreeView interaction in Widget.

Replace `noteRealPath(note)` calls with `m_noteFileService->noteRealPath(note)`.
Replace `getWorkshopNoteStrIdFromPath(path)` calls with `m_noteFileService->noteStrIdFromWorkshopPath(path)`.

- [ ] **Step 5: Update CMakeLists.txt**

Add:
```cmake
NoteFileService.cpp NoteFileService.h
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build build
```

Run and test: create a new note, create a new folder, trash a note, verify files on disk are correct.

---

### Task 4: Extract SearchController

**Files:**
- Create: `SearchController.h`
- Create: `SearchController.cpp`
- Modify: `Widget.h`
- Modify: `Widget.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create SearchController.h**

```cpp
#ifndef MYNOTES_SEARCHCONTROLLER_H
#define MYNOTES_SEARCHCONTROLLER_H

#include <QObject>
#include <QString>
#include "DbModel.h"

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

    void initSearchDialog(QWidget* parent);
    void showSearchDialog(const QRect& parentGeometry);
    void hideSearchDialog();
    void initIndexer();  // rebuild full index

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
```

- [ ] **Step 2: Create SearchController.cpp**

Move these methods from Widget.cpp:

| From Widget.cpp | Notes |
|---|---|
| `initSearchDialog()` (line 746-761) | `m_searchDialog` parent is now passed as parameter; connect `searchTextChanged` and `clickNote` to internal slots |
| `on_searchDialog_searchTextChanged()` (line 763-834) | Async search with `QtConcurrent::run` → `Indexer::search` → `DbManager::getNote` → fill paths → `SearchDialog::setSearchResult`. Keep the `#if 1` code path, drop the `#if 0` dead code. Connect `clickNote` to internal slot that emits `noteSelected(int noteId)`. |
| `initIndexer()` (line 852-886) | Rebuild full index from all notes |
| `searchResultView()` (line 888-898) | Keep inline since it creates the ListView — this is internal to SearchController now |

When `clickNote` is received from the SearchDialog, emit `noteSelected(noteId)` instead of directly calling `this->loadNote(note)` (the old code). The Widget will connect `noteSelected` to the editor.

- [ ] **Step 3: Update Widget.h**

Remove:
```cpp
void initSearchDialog();
void on_searchDialog_searchTextChanged(const QString& text);
void initIndexer();
QListView* searchResultView();
SearchDialog* m_searchDialog;
```

Add:
```cpp
class SearchController;
SearchController* m_searchController;
```

- [ ] **Step 4: Update Widget.cpp — Wire SearchController**

In constructor:
```cpp
m_searchController = new SearchController(m_dbManager, m_indexer, m_notesPath, this);
connect(m_searchController, &SearchController::noteSelected, this, [this](int noteId) {
    auto note = m_dbManager->getNote(noteId);
    if (note.id() != -1) {
        loadNote(note);  // will become m_noteEditorWidget->loadNote in Task 6
    }
});
```

In `eventFilter()` (Shift双击):
```cpp
if (event->key() == Qt::Key_Shift) {
    auto curTime = Utils::getTimeStamp();
    if (curTime - m_lastPressShiftTime < m_maxShiftInterval) {
        m_lastPressShiftTime = curTime;
        m_searchController->showSearchDialog(this->geometry());
    } else {
        m_lastPressShiftTime = curTime;
    }
}
```

In `initFileSystemWatcher()` — connect `requestReindex` from SettingsDialog:
This was previously connected via `connect(&dialog, &SettingsDialog::requestReindex, ...)` in the tray menu. Move that connection to where SettingsDialog is opened (both in settings action and tray), or keep the Widget as the intermediary.

- [ ] **Step 5: Update CMakeLists.txt**

Add:
```cmake
SearchController.cpp SearchController.h
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build build
```

Run and test: double-tap Shift, search for a note, click a result — note should open.

---

### Task 5: Extract TrayIconManager

**Files:**
- Create: `TrayIconManager.h`
- Create: `TrayIconManager.cpp`
- Modify: `Widget.h`
- Modify: `Widget.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create TrayIconManager.h**

```cpp
#ifndef MYNOTES_TRAYICONMANAGER_H
#define MYNOTES_TRAYICONMANAGER_H

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class Settings;

class TrayIconManager : public QObject {
    Q_OBJECT
public:
    explicit TrayIconManager(Settings* settings, QObject *parent = nullptr);
    void setup(QWidget* mainWindow);

signals:
    void showRequested();
    void settingsRequested();
    void aboutRequested();
    void quitRequested();
    void trojanToggled(bool enabled);
    void reindexRequested();
    void syncAllRequested();

private:
    void setAutoStart();
    QSystemTrayIcon* m_trayIcon;
    Settings* m_settings;
};

#endif
```

- [ ] **Step 2: Create TrayIconManager.cpp**

Move the entire `initSystemTrayIcon()` method (lines 1162-1246) and `setAutoStart()` (lines 1573-1603) from Widget.cpp into this file. 

Key change: instead of the tray menu actions directly calling `this->showNormal()`, `qApp->quit()`, etc., they emit signals: `showRequested()`, `quitRequested()`, etc. The Widget connects these signals.

The Trojan toggle logic stays inside TrayIconManager since it manages the tray menu. Emit `trojanToggled(bool)` for the Widget to handle.

For the Settings action emit `settingsRequested()` instead of directly creating a SettingsDialog. For About, emit `aboutRequested()`.

- [ ] **Step 3: Update Widget.h**

Remove:
```cpp
void initSystemTrayIcon();
void setAutoStart();
QSystemTrayIcon* m_systemTrayIcon;
```

Add:
```cpp
class TrayIconManager;
TrayIconManager* m_trayIconManager;
```

- [ ] **Step 4: Update Widget.cpp — Wire TrayIconManager**

In constructor:
```cpp
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
```

Remove `initSystemTrayIcon()` call from constructor (replaced by `m_trayIconManager->setup(this)`).
Remove `setAutoStart()` call from constructor (now called inside `TrayIconManager::setup()`).
Remove `m_systemTrayIcon` references in `closeEvent` — replace with `m_trayIconManager` check (or keep the hide-to-tray behavior by having `TrayIconManager` expose `bool isVisible()`).

- [ ] **Step 5: Update CMakeLists.txt**

Add:
```cmake
TrayIconManager.cpp TrayIconManager.h
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build build
```

Run and test: system tray icon appears, click show/settings/about/quit all work.

---

### Task 6: Extract NoteEditorWidget ★

**Files:**
- Create: `NoteEditorWidget.h`
- Create: `NoteEditorWidget.cpp`
- Modify: `Widget.h`
- Modify: `Widget.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create NoteEditorWidget.h**

```cpp
#ifndef MYNOTES_NOTEEDITORWIDGET_H
#define MYNOTES_NOTEEDITORWIDGET_H

#include <QWidget>
#include "DbModel.h"

class QTextEdit;
class TabWidget;
class TextPreview;
class Indexer;

class NoteEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit NoteEditorWidget(QWidget *parent = nullptr);

    void setIndexer(Indexer* indexer);
    void loadNote(const Note& note, const QString& realPath);
    void loadNote(const QString& filePath);
    void save();
    void refreshPreview();
    QString currentFilePath() const;
    void showNextTab();
    void openInTypora(const QString& path);
    void reloadTabAt(const QString& path);

signals:
    void noteModified(const QString& path);
    void linkClicked(const QString& link);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void loadMdText(const QString& path);
    void updatePreview(const QString& path);
    void updateIndex(const QString& text, int noteId);

    QTextEdit *m_textEdit;
    TabWidget *m_tabWidget;
    Note m_curNote;
    QString m_curNotePath;
    Indexer* m_indexer;
};

#endif
```

- [ ] **Step 2: Create NoteEditorWidget.cpp**

Move these methods from Widget.cpp into NoteEditorWidget.cpp:

| From Widget | To NoteEditorWidget |
|---|---|
| `loadNote(const Note&)` (line 911-920) | Rename to `loadNote(const Note&, const QString& realPath)`, remove the Toast |
| `loadNote(const QString&)` (line 922-933) | `loadNote(const QString&)` — add path to FileSystemWatcher, call loadMdText + updatePreview |
| `loadMdText()` / `loadMdText(const QString&)` (line 650-672) | Keep only `loadMdText(const QString&)`, make private |
| `saveMdText()` (line 554-566) | Rename to `save()`, keep logic |
| `updatePreview()` (line 477-479) | Delegate to `updatePreview(const QString&)` |
| `updatePreview(const QString&)` (line 481-551) | `updatePreview(const QString&)` — the tab management logic |
| `currentNotePath()` (line 1008-1010) | `currentFilePath()` |
| `updateIndex()` (line 725-744) | Make private, keep logic |
| `updateStatistics()` (line 936-952) | Keep the empty/placeholder method |
| `openInTypora()` (line 959-988) | `openInTypora(const QString&)` |
| `openNoteInTypora()` (line 990-992) | Remove (caller uses `openInTypora(path)`) |
| `showNextTab()` (line 1819-1823) | Keep, same logic |
| `reloadTabAt(path)` | New: from `on_fileSystemWatcher_fileChanged` — iterate tabs, call `tab->reload()` if path matches |
| `on_fileSystemWatcher_fileChanged()` tab reload part (line 1262-1266) | Move to `reloadTabAt(path)` |
| Constructor parts (line 101-113) | Move m_textEdit, m_tabWidget creation here |
| `loadLastOpenedNote()` (line 175-205) | Move here |

In the constructor, create `m_textEdit`, `m_tabWidget`, and set up the splitter layout. Connect `m_tabWidget->tabCloseRequested` to remove the tab.

The `eventFilter` handles the `Ctrl+E` shortcut to open in Typora (line 417-420 in old Widget::eventFilter).

Note: `loadNote` currently calls `m_fileSystemWatcher->addPath(path)`. Since FileSystemWatcher is a singleton, it's fine to call from here.

- [ ] **Step 3: Update Widget.h — Remove editing members**

Remove these member variables:
```cpp
QTextEdit *m_textEdit;
TabWidget *m_tabWidget;
Note m_curNote;
QString m_curNotePath;
```

Remove these methods:
```cpp
void loadNote(const Note& note);
void loadNote(const QString& path);
void loadMdText();
void loadMdText(const QString& path);
void saveMdText();
void updatePreview();
void updatePreview(const QString& path);
void updateIndex(QString text, int id);
void updateStatistics();
void openNoteInTypora(const Note& note);
void openInTypora(const QString& path);
void loadLastOpenedNote();
QString currentNotePath();
```

Add:
```cpp
class NoteEditorWidget;
NoteEditorWidget* m_noteEditorWidget;
```

- [ ] **Step 4: Update Widget.cpp — Wire NoteEditorWidget**

In the constructor, replace the old m_textEdit/m_tabWidget creation with:
```cpp
m_noteEditorWidget = new NoteEditorWidget();
m_noteEditorWidget->setIndexer(m_indexer);
```

Replace the splitter content:
```cpp
splitter->addWidget(m_noteEditorWidget);  // instead of m_tabWidget
```

Replace `loadNote(note)` calls with `m_noteEditorWidget->loadNote(note, m_noteFileService->noteRealPath(note))`.
Replace `loadNote(path)` calls with `m_noteEditorWidget->loadNote(path)`.
Replace `saveMdText()` / `updatePreview()` calls with `m_noteEditorWidget->save()` etc.
Replace `showNextTab()` with `m_noteEditorWidget->showNextTab()`.

In `eventFilter`, replace `saveMdText(); updatePreview();` with `m_noteEditorWidget->save(); m_noteEditorWidget->refreshPreview();`.
In `eventFilter`, replace the `Qt::Key_E` / `openInTypora(currentNotePath())` with `m_noteEditorWidget->openInTypora(m_noteEditorWidget->currentFilePath())`.

In `on_fileSystemWatcher_fileChanged`, replace the tab reload loop with `m_noteEditorWidget->reloadTabAt(path)`.

In `on_treeView_pressed`, replace `currentNotePath()` calls with `m_noteEditorWidget->currentFilePath()`.

Remove `m_curNote`, `m_curNotePath`, `m_textEdit` references throughout.

Update `on_action_openInTypora()`:
```cpp
void Widget::on_action_openInTypora() {
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) return;
    auto item = static_cast<NoteItem *>(index.internalPointer());
    if (!item) { showErrorDialog(tr("open in typora fail")); return; }
    m_noteEditorWidget->openInTypora(m_noteFileService->noteRealPath(item->note()));
}
```

Update `loadLastOpenedNote()` call: move to `NoteEditorWidget` constructor (or keep in Widget constructor but delegate to editor).

After this step, Widget.cpp drops from ~1818 lines to approximately 200-250 lines.

- [ ] **Step 5: Update CMakeLists.txt**

Add:
```cmake
NoteEditorWidget.cpp NoteEditorWidget.h
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build build
```

Run and test full workflow: create note → edit → Ctrl+S save → open another note in tree → tab switching → Ctrl+Tab → close tab → search → export HTML.

---

### Task 7: Final cleanup and verify mobile build

**Files:**
- Modify: `Widget.h` — final cleanup of unused forward declarations
- Modify: `Widget.cpp` — final cleanup
- Modify: `CMakeLists.txt` — verify all sources listed

- [ ] **Step 1: Clean up Widget.h**

Remove now-unused `#include` and forward declarations. The final Widget.h should only include:
```cpp
#include <QWidget>
#include "PiWidget.h"
// Forward declarations for owned components
class QTreeView;
class TreeModel;
class DbManager;
class Indexer;
class FileSystemWatcher;
class Settings;
class NoteFileService;
class HtmlExporter;
class SyncService;
class SearchController;
class TrayIconManager;
class NoteEditorWidget;
class QTimer;
```

Remove all moved method declarations. Keep only:
```cpp
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showNextTab(); // delegate to m_noteEditorWidget

protected:
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_treeView_pressed(const QModelIndex &index);
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_action_newNote();
    void on_action_newFolder();
    void on_action_trashNote();
    void on_action_openInTypora();
    void on_action_trashFolder();
    void on_action_exportNoteToHTML();

private:
    inline TreeItem* currentTreeItem();
    inline void showErrorDialog(const QString &msg);
    void loadLastOpenedNote();
    void initFileSystemWatcher();

    QTreeView *m_treeView;
    TreeModel *m_treeModel;
    DbManager* m_dbManager;
    Indexer* m_indexer;
    NoteEditorWidget* m_noteEditorWidget;
    NoteFileService* m_noteFileService;
    HtmlExporter* m_htmlExporter;
    SyncService* m_syncService;
    SearchController* m_searchController;
    TrayIconManager* m_trayIconManager;
    Settings *m_settings;
    FileSystemWatcher* m_fileSystemWatcher;
    QTimer* m_timer;
    QString m_notesPath;
    time_t m_lastPressShiftTime;
    time_t m_maxShiftInterval;
    bool m_showOpenInTyporaTip;
};
```

- [ ] **Step 2: Verify CMakeLists.txt**

The `target_sources(libMyNotes PRIVATE ...)` should now include all new files:
```cmake
WatchingFileHtmlVisitor.cpp WatchingFileHtmlVisitor.h
HtmlExporter.cpp HtmlExporter.h
SyncService.cpp SyncService.h
NoteFileService.cpp NoteFileService.h
SearchController.cpp SearchController.h
TrayIconManager.cpp TrayIconManager.h
NoteEditorWidget.cpp NoteEditorWidget.h
```

- [ ] **Step 3: Full build**

```bash
cmake --build build
```

Fix any remaining compilation errors. Run `./build/MyNotes` and go through the full feature checklist:
- [ ] Create note — works
- [ ] Create folder — works  
- [ ] Trash note — works
- [ ] Open in Typora (press E) — works
- [ ] Export to HTML — works
- [ ] Double-Shift search — works
- [ ] Ctrl+S save — works
- [ ] Ctrl+Tab switch tab — works
- [ ] System tray show/hide/quit — works
- [ ] Right-click context menus — all work
- [ ] FileSystemWatcher detects changes — works
- [ ] Note loads on tree click — works

- [ ] **Step 4: Commit**

```bash
git add -A
git commit -m "refactor: split Widget into 7 single-responsibility classes"
```

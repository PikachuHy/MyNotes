# Widget 拆分设计

日期: 2026-05-01
目标: 将 Widget (1818行 God Object) 拆分为 7 个职责单一的类，为增强编辑/预览能力铺路

## 动机

- Widget.cpp 承担了 UI 布局、笔记 CRUD、搜索、同步、HTML 导出、系统托盘等过多职责
- 计划增强编辑/预览功能（语法高亮、实时预览等），但不敢往已经 1800 行的 Widget 里塞代码
- 需要一步到位的重构，不拖泥带水

## 拆分方案: Document 驱动 + 服务提取

Widget 从 God Object 降级为 ~200 行的 Shell，所有具体逻辑下沉到各自的类中。

### 新增的类

#### 1. NoteEditorWidget (继承 QWidget) — 编辑/预览管线 ★ 核心
封装笔记的 **编辑 → 预览 → 保存** 流程。这是未来加语法高亮、实时预览的主要修改点。

- 内部持有 TabWidget + TextPreview 组合
- 暴露接口: loadNote(path), save(), refreshPreview(), currentFilePath()
- 发出信号: noteModified(path), linkClicked(url)
- 从 Widget 移入: loadNote, loadMdText, saveMdText, updatePreview, currentNotePath, generateHTML 相关方法, WatchingFileHtmlVisitor

#### 2. NoteFileService (继承 QObject) — 笔记文件 CRUD
封装笔记/文件夹的创建、删除、路径解析。

- 依赖: DbManager
- 暴露接口: createNote(name, parentPathId) → Note, createFolder(name, parentId) → Path, trashNote(id), trashFolder(id), noteRealPath(note) → QString
- 从 Widget 移入: on_action_newNote, on_action_newFolder, on_action_trashNote, on_action_trashFolder, noteRealPath, addNoteTo

#### 3. SyncService (继承 QObject) — 同步/上传
所有网络同步逻辑。依赖 ElasticSearchRestApi、Http、HtmlExporter。(fileChecksum 作为静态辅助函数)

- 依赖: ElasticSearchRestApi, Http, Settings, HtmlExporter (调用 generateHTML)
- 暴露接口: syncAll(), syncWatchingFolder(path), syncWatchingFile(path), syncWorkshopFile(note), uploadNoteAttachment(note), uploadFile(noteId, path), updateProfile()

#### 4. SearchController (继承 QObject) — 搜索编排
管理 SearchDialog 生命周期和搜索流程。

- 依赖: Indexer, SearchDialog, DbManager
- 暴露接口: showSearchDialog(), hideSearchDialog(), search(text)
- 发出信号: noteSelected(noteId)

#### 5. HtmlExporter (继承 QObject) — HTML 导出
独立的 HTML 生成和导出逻辑。

- 依赖: QtMarkdownParser
- 暴露接口: exportToHtml(note, dirPath), generateHtml(note) → QString
- 从 Widget 移入: generateHTML (3个重载), on_action_exportNoteToHTML, fileChecksum, WatchingFileHtmlVisitor (独立文件)

#### 6. TrayIconManager (继承 QObject) — 系统托盘
封装系统托盘的创建和菜单构建。

- 依赖: Settings, TrojanThread (可选, 编译时)
- 暴露接口: setup()
- 发出信号: showRequested(), settingsRequested(), quitRequested()

#### 7. Widget — Shell/协调器 (~200行)
只负责: 创建和管理所有子组件、连接信号槽、右键菜单构建、快捷键/事件过滤

### 信号槽连接 (Widget 构造函数中完成)

| 发送者 | 信号 | 接收者 | 槽 |
|--------|------|--------|-----|
| TreeView | pressed | Widget | on_treeView_pressed |
| SearchDialog | searchTextChanged | SearchController | search |
| SearchDialog | clickNote | Widget | → loadNote |
| FileSystemWatcher | fileChanged | NoteEditorWidget | reload tab |
| FileSystemWatcher | fileChanged | SyncService | auto sync |
| FileSystemWatcher | renameFolder/newFolder/deleteFolder | Widget | → TreeModel update |
| TrayIconManager | showRequested | Widget | showNormal |
| TrayIconManager | quitRequested | QApplication | quit |
| TabWidget | tabCloseRequested | NoteEditorWidget | removeTab |
| SettingsDialog | requestReindex | SearchController | initIndexer |

### 核心交互流程

**打开笔记:** TreeView::pressed → Widget::on_treeView_pressed → NoteFileService::noteRealPath → NoteEditorWidget::loadNote
**保存笔记:** Widget eventFilter 捕获 Ctrl+S → NoteEditorWidget::save → Indexer::updateIndex
**搜索笔记:** Shift双击 → SearchController::showSearchDialog → SearchDialog::searchTextChanged → SearchController::search → Indexer::search → DbManager → SearchDialog::setSearchResult
**同步笔记:** 定时器/右键菜单 → SyncService → HtmlExporter → ElasticSearchRestApi/Http

## 实施步骤 (按依赖顺序)

### 步骤 1: 提取 HtmlExporter
依赖: 无新类。纯逻辑提取，WatchingFileHtmlVisitor 独立文件。预计 ~200 行。

### 步骤 2: 提取 SyncService
依赖: HtmlExporter (调用 generateHTML)。所有 sync/upload/profile 方法移入。预计 ~300 行。

### 步骤 3: 提取 NoteFileService
依赖: DbManager。CRUD 操作移入。预计 ~150 行。

### 步骤 4: 提取 SearchController
依赖: Indexer, SearchDialog, DbManager。搜索流程编排。预计 ~120 行。

### 步骤 5: 提取 TrayIconManager
依赖: Settings。托盘和菜单。预计 ~100 行。

### 步骤 6: 提取 NoteEditorWidget ★
依赖: TabWidget, TextPreview, Indexer。最关键一步。完成后 Widget 缩减到 ~200 行。预计 ~250 行。

### 步骤 7: 更新 CMakeLists.txt
新文件加入 libMyNotes target_sources。Mobile 入口不经过 Widget，不受影响。

每步完成后编译通过 + 运行桌面版验证基本功能。

## 不做的事

- 不引入新测试框架
- 不改变移动端 (mobile/) 代码
- 不改变数据库 schema
- 不改变文件存储结构
- 不拆右键菜单（保留在 Widget，因为涉及多组件交互）

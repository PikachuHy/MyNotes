# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure and build (desktop)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# With tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TEST=ON
cmake --build build

# With WebEngineView for markdown preview
cmake -B build -DUSE_WEB_ENGINE_VIEW=ON

# Run a single test
./build/test/IndexerTest

# Update translations
cmake -B build -DUPDATE_TS=ON

# macOS package (after building with MinSizeRel)
./mac_package.sh
```

CMake options: `USE_WEB_ENGINE_VIEW` (markdown preview via QWebEngineView), `ENABLE_TROJAN` (proxy support), `BUILD_WITH_QT_QUICK` (Android/mobile target), `BUILD_TEST`, `UPDATE_TS`.

Qt5 and Qt6 are both supported — the build detects `QT_VERSION_MAJOR` to switch APIs.

## Architecture

MyNotes is a cross-platform (macOS, Windows, Android) note management app. Notes are markdown files on disk, tracked by a SQLite database for metadata and full-text search.

**Entry points:**
- `desktop/main.cpp` — Desktop app. Sets up logging, checks login/config, then creates a `MainWindow` containing the central `Widget`.
- `mobile/main.cpp` — Android app. Uses Qt Quick/QML; exposes `TreeModel`, `DbManager`, and `KeyFilter` as QML context properties.

**Core library (`libMyNotes`):** Shared between desktop and mobile targets. Contains all models, database access, indexing, and networking.

**Key classes:**
- `Widget` (inherits `PiWidget` → `QWidget`) — The main desktop UI. Composes a folder `TreeView` (left), a note `ListView`, a `TabWidget` of open notes (center), and a `WebEngineView`/`TextEdit` for preview/editing. Owns the `DbManager`, `Indexer`, `FileSystemWatcher`, and `ElasticSearchRestApi`.
- `DbManager` — SQLite wrapper. Manages `Note` and `Path` tables (see `DbModel.h`). Provides methods for CRUD, full-text search, and index updates.
- `Indexer` — Uses jieba (via `QtJieba::Tokenizer`) for Chinese word segmentation. Builds an inverted index (`QMap<QString, int>` word dict) persisted to disk under the notes directory. Supports load/save/search.
- `TreeModel` / `TreeItem` — QAbstractItemModel for the folder tree. Populated from `DbManager` path queries.
- `ListModel` — Model for the note list view (flat list of notes in a folder).
- `TabWidget` — Holds multiple `TextPreview` tabs, each rendering a markdown note.
- `FileSystemWatcher` — Singleton wrapping `QFileSystemWatcher`. Detects file/folder renames, deletions, and creations within the notes directory tree.
- `Settings` — Singleton wrapping `QSettings` (INI format). Uses a templated `Ref` pattern (`QStringRef<key>`, `BoolRef<key>`, etc.) for typed access. Stores server IP, user credentials, window geometry, sync preferences, etc.
- `SingleApplication` — Ensures only one app instance via `QSharedMemory`. Sends activation messages to the existing instance.
- `ElasticSearchRestApi` — Remote search/sync via HTTP to a server-side Elasticsearch.
- `TrojanThread` — Optional proxy thread (compile-time `ENABLE_TROJAN` gate).
- `LoginApi` / `LoginDialog` — Authenticate against a remote server; credentials are cached in `Settings` with an MD5 signature.

**Data flow:**
1. Notes live as `.md` files under `~/Documents/MyNotes/workshop/`, organized in subdirectories matching the folder tree.
2. `FileSystemWatcher` detects external changes and syncs them to SQLite via `DbManager`.
3. On note save/edit, `Indexer` tokenizes the text and updates the inverted index.
4. `WebEngineView` renders markdown to HTML for preview (optional, gated by `USE_WEB_ENGINE_VIEW`).
5. Remote sync pushes notes to a server via `ElasticSearchRestApi`.

**Dependencies** (git submodules in `deps/`):
- `CuteLogger` — Desktop-only logging framework
- `QtJieba` — Chinese text segmentation
- `QtMarkdown` — Markdown parser and editor widgets
- `QtWordReader` — Word document reader

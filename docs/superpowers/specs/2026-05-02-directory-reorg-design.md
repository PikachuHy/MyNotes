# Directory Reorganization Design

**Date:** 2026-05-02
**Status:** Approved

## Goal

Reorganize ~41 classes currently in the repository root into a two-level directory structure under `src/`, grouped by cohesion (what changes together). `desktop/`, `mobile/`, `test/`, `deps/`, and `assets/` remain at root.

## Directory Structure

```
src/
├── database/          DbManager(.cpp/.h), DbModel(.cpp/.h)
├── indexer/           Indexer(.cpp/.h)
├── notes/             NoteFileService(.cpp/.h)
├── sync/              SyncService(.cpp/.h), ElasticSearchRestApi(.cpp/.h)
├── auth/              LoginApi(.cpp/.h), LoginDialog(.cpp/.h)
├── network/           Http(.cpp/.h), TrojanThread(.cpp/.h)
├── watcher/           FileSystemWatcher(.cpp/.h)
├── settings/          Settings(.cpp/.h)
├── appcore/           SingleApplication(.cpp/.h), TrayIconManager(.cpp/.h)
├── widgets/           Widget(.cpp/.h), MainWindow(.cpp/.h), PiWidget(.cpp/.h),
│                      PiDialog(.cpp/.h)
├── editor/            NoteEditorWidget(.cpp/.h), TextPreview(.cpp/.h),
│                      TabWidget(.cpp/.h), TabBar(.cpp/.h),
│                      MarkdownHighlighter(.cpp/.h), WebEngineView(.cpp/.h)
├── navigation/        TreeView(.cpp/.h), TreeModel(.cpp/.h), TreeItem(.cpp/.h),
│                      ListView(.cpp/.h), ListModel(.cpp/.h)
├── search/            SearchController(.cpp/.h), SearchDialog(.cpp/.h)
├── dialogs/           SettingsDialog(.cpp/.h), AboutDialog(.cpp/.h),
│                      ChooseFolderWidget(.cpp/.h)
├── export/            HtmlExporter(.cpp/.h), WatchingFileHtmlVisitor(.cpp/.h)
└── utils/             Utils(.cpp/.h), Constant(.cpp/.h), Singleton.h, Toast(.cpp/.h)
```

Unmoved: `desktop/`, `mobile/`, `test/`, `deps/`, `assets/` stay at root.

Root also retains: `CMakeLists.txt`, `config.h.in`, `Info.plist`, `mac_package.sh`, `mac_package_debug.sh`, `README.md`, `MyNotes_zh_CN.ts`.

## CMake Changes

### Strategy

Each subdirectory gets its own `CMakeLists.txt` that adds its sources to `libMyNotes`. The root `CMakeLists.txt` uses `add_subdirectory()` to include them.

### Per-subdirectory CMakeLists.txt (example: src/database/)

```cmake
target_sources(libMyNotes PRIVATE
    DbManager.cpp DbManager.h
    DbModel.cpp DbModel.h
)
```

All 16 subdirectories follow this pattern.

### Root CMakeLists.txt changes

Replace the monolithic `target_sources(libMyNotes PRIVATE ...)` block with:

```cmake
target_sources(libMyNotes PRIVATE
    MyNotes_zh_CN.ts
)
add_subdirectory(src/database)
add_subdirectory(src/indexer)
add_subdirectory(src/notes)
add_subdirectory(src/sync)
add_subdirectory(src/auth)
add_subdirectory(src/network)
add_subdirectory(src/watcher)
add_subdirectory(src/settings)
add_subdirectory(src/appcore)
add_subdirectory(src/widgets)
add_subdirectory(src/editor)
add_subdirectory(src/navigation)
add_subdirectory(src/search)
add_subdirectory(src/dialogs)
add_subdirectory(src/export)
add_subdirectory(src/utils)
```

qrc files (`assets/db.qrc`, `assets/css.qrc`, `assets/icon.qrc`) move to the same root `target_sources` call alongside the `.ts` file.

Existing `add_subdirectory(deps)` and `add_subdirectory(test)` remain unchanged. `target_link_libraries`, `target_compile_definitions`, and `USE_WEB_ENGINE_VIEW` conditional blocks stay as-is.

## Include Path Changes

Add `src/` to the include path:

```cmake
target_include_directories(libMyNotes PUBLIC ${CMAKE_SOURCE_DIR}/src)
```

All `#include` statements change from bare filenames to directory-qualified paths:

```cpp
// Before
#include "DbManager.h"
#include "Widget.h"

// After
#include "database/DbManager.h"
#include "widgets/Widget.h"
```

Same-directory includes also use the qualified form for consistency. `desktop/main.cpp` and `mobile/main.cpp` use this style since the executable targets link against `libMyNotes` with `PUBLIC` include propagation.

## Scope

- Move 82 source files (41 .cpp + 41 .h) into `src/` subdirectories
- Create 16 `CMakeLists.txt` files
- Update root `CMakeLists.txt`
- Update ~150-200 `#include` statements across all files
- No behavioral changes, no logic modifications

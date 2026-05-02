# Directory Reorganization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Move 41 classes (82 source files) from repo root into a two-level `src/` directory structure grouped by cohesion, with per-directory CMakeLists.txt files.

**Architecture:** 16 subdirectories under `src/`, each with its own `CMakeLists.txt`. Root `CMakeLists.txt` has a transitional monolithic `target_sources` that shrinks as files move, replaced incrementally by `add_subdirectory` calls. Tasks are ordered by dependency depth — a directory only moves after all headers it includes have already moved to `src/`. All `#include` statements change from bare filenames to directory-qualified paths.

**Tech Stack:** CMake 3.5+, C++17, Qt5/Qt6

**Key constraint:** No behavioral changes. Build must pass after every directory move task.

---

### Task 1: Create directory structure and add include path

**Files:**
- Create: `src/` and 16 subdirectories
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create all src/ subdirectories**

```bash
mkdir -p src/{database,indexer,notes,sync,auth,network,watcher,settings,appcore,widgets,editor,navigation,search,dialogs,export,utils}
```

- [ ] **Step 2: Add target_include_directories to root CMakeLists.txt**

After the `add_library(libMyNotes "")` line (~line 30), insert:

```cmake
target_include_directories(libMyNotes PUBLIC ${CMAKE_SOURCE_DIR}/src)
```

- [ ] **Step 3: Build to verify nothing broken**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Expected: build succeeds (no files moved, monolithic target_sources intact, include path addition is harmless).

- [ ] **Step 4: Commit**

```bash
git add -A
git commit -m "build: create src/ directory structure and add include path"
```

---

### Task 2: Move utils/ (leaf — no project deps)

**Files:**
- Create: `src/utils/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `Utils.cpp`, `Utils.h`, `Constant.cpp`, `Constant.h`, `Toast.cpp`, `Toast.h`, `Singleton.h` → `src/utils/`

- [ ] **Step 1: Create src/utils/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    Utils.cpp Utils.h
    Constant.cpp Constant.h
    Toast.cpp Toast.h
)
```

Singleton.h is header-only — omitted from target_sources.

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/utils)` after the `target_sources(libMyNotes PRIVATE ...)` block.

Remove from the monolithic target_sources: `Utils.cpp`, `Constant.cpp`, `Toast.cpp`

- [ ] **Step 3: Move files**

```bash
git mv Utils.cpp Utils.h Constant.cpp Constant.h Toast.cpp Toast.h Singleton.h src/utils/
```

- [ ] **Step 4: Update includes in moved files**

```bash
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/utils/Utils.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/utils/Constant.cpp
sed -i '' 's|#include "Toast.h"|#include "utils/Toast.h"|' src/utils/Toast.cpp
```

- [ ] **Step 5: Update all external references**

```bash
grep -rl '#include "Utils.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|g'
grep -rl '#include "Constant.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|g'
grep -rl '#include "Toast.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Toast.h"|#include "utils/Toast.h"|g'
grep -rl '#include "Singleton.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Singleton.h"|#include "utils/Singleton.h"|g'
```

- [ ] **Step 6: Build to verify**

```bash
cmake --build build
```

Expected: build succeeds. Moved files compile via add_subdirectory, remaining files via root target_sources.

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "refactor: move utils/ to src/utils/"
```

---

### Task 3: Move settings/ (leaf — no project deps)

**Files:**
- Create: `src/settings/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `Settings.cpp`, `Settings.h` → `src/settings/`

- [ ] **Step 1: Create src/settings/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    Settings.cpp Settings.h
)
```

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/settings)`. Remove `Settings.cpp` from root target_sources.

- [ ] **Step 3: Move files and update includes**

```bash
git mv Settings.cpp Settings.h src/settings/
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/settings/Settings.cpp
grep -rl '#include "Settings.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|g'
```

- [ ] **Step 4: Build to verify**

```bash
cmake --build build
```

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "refactor: move settings/ to src/settings/"
```

---

### Task 4: Move database/ (depends on utils/)

**Files:**
- Create: `src/database/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `DbManager.cpp`, `DbManager.h`, `DbModel.cpp`, `DbModel.h` → `src/database/`

- [ ] **Step 1: Create src/database/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    DbManager.cpp DbManager.h
    DbModel.cpp DbModel.h
)
```

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/database)`. Remove `DbManager.cpp`, `DbModel.cpp` from root target_sources.

- [ ] **Step 3: Move files**

```bash
git mv DbManager.cpp DbManager.h DbModel.cpp DbModel.h src/database/
```

- [ ] **Step 4: Update includes in moved files**

```bash
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/database/DbManager.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/database/DbManager.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/database/DbManager.h
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/database/DbModel.cpp
```

- [ ] **Step 5: Update all external references**

```bash
grep -rl '#include "DbManager.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|g'
grep -rl '#include "DbModel.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|g'
```

DbManager.h affected: Indexer.cpp, NoteEditorWidget.cpp, NoteFileService.cpp, SearchController.cpp, SyncService.cpp, TreeModel.cpp, Widget.cpp, ChooseFolderWidget.h, mobile/main.cpp

DbModel.h affected: ElasticSearchRestApi.h, HtmlExporter.h, ListModel.h, NoteEditorWidget.h, NoteFileService.h, SyncService.h, TreeItem.h, Widget.h

- [ ] **Step 6: Build to verify**

```bash
cmake --build build
```

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "refactor: move database/ to src/database/"
```

---

### Task 5: Move watcher/ (leaf — no project deps)

**Files:**
- Create: `src/watcher/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `FileSystemWatcher.cpp`, `FileSystemWatcher.h` → `src/watcher/`

- [ ] **Step 1: Create src/watcher/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    FileSystemWatcher.cpp FileSystemWatcher.h
)
```

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/watcher)`. Remove `FileSystemWatcher.cpp` from root target_sources.

- [ ] **Step 3: Move files and update includes**

```bash
git mv FileSystemWatcher.cpp FileSystemWatcher.h src/watcher/
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/watcher/FileSystemWatcher.cpp
grep -rl '#include "FileSystemWatcher.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|g'
```

- [ ] **Step 4: Build to verify**

```bash
cmake --build build
```

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "refactor: move watcher/ to src/watcher/"
```

---

### Task 6: Move network/ (leaf — no project deps)

**Files:**
- Create: `src/network/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `Http.cpp`, `Http.h`, `TrojanThread.cpp`, `TrojanThread.h` → `src/network/`

- [ ] **Step 1: Create src/network/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    Http.cpp Http.h
    TrojanThread.cpp TrojanThread.h
)
```

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/network)`. Remove `Http.cpp`, `TrojanThread.cpp` from root target_sources.

- [ ] **Step 3: Move files and update includes**

```bash
git mv Http.cpp Http.h TrojanThread.cpp TrojanThread.h src/network/
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/network/Http.cpp
sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|' src/network/TrojanThread.cpp
grep -rl '#include "Http.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Http.h"|#include "network/Http.h"|g'
grep -rl '#include "TrojanThread.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|g'
```

- [ ] **Step 4: Build to verify**

```bash
cmake --build build
```

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "refactor: move network/ to src/network/"
```

---

### Task 7: Move widgets/ (depends on database/, settings/; auth/dialogs need PiWidget/PiDialog moved first)

**Files:**
- Create: `src/widgets/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Move: `Widget.cpp`, `Widget.h`, `MainWindow.cpp`, `MainWindow.h`, `PiWidget.cpp`, `PiWidget.h`, `PiDialog.cpp`, `PiDialog.h` → `src/widgets/`

- [ ] **Step 1: Create src/widgets/CMakeLists.txt**

```cmake
target_sources(libMyNotes PRIVATE
    Widget.cpp Widget.h
    MainWindow.cpp MainWindow.h
    PiWidget.cpp PiWidget.h
    PiDialog.cpp PiDialog.h
)
```

- [ ] **Step 2: Update root CMakeLists.txt**

Add `add_subdirectory(src/widgets)`. Remove `Widget.cpp`, `MainWindow.cpp`, `PiWidget.cpp`, `PiDialog.cpp` from root target_sources.

- [ ] **Step 3: Move files**

```bash
git mv Widget.cpp Widget.h MainWindow.cpp MainWindow.h PiWidget.cpp PiWidget.h PiDialog.cpp PiDialog.h src/widgets/
```

- [ ] **Step 4: Update includes in moved files**

```bash
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/Widget.h
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "NoteFileService.h"|#include "notes/NoteFileService.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SyncService.h"|#include "sync/SyncService.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeItem.h"|#include "navigation/TreeItem.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeModel.h"|#include "navigation/TreeModel.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeView.h"|#include "navigation/TreeView.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TrayIconManager.h"|#include "appcore/TrayIconManager.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "NoteEditorWidget.h"|#include "editor/NoteEditorWidget.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SearchController.h"|#include "search/SearchController.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "ElasticSearchRestApi.h"|#include "sync/ElasticSearchRestApi.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SettingsDialog.h"|#include "dialogs/SettingsDialog.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "AboutDialog.h"|#include "dialogs/AboutDialog.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "HtmlExporter.h"|#include "export/HtmlExporter.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "MainWindow.h"|#include "widgets/MainWindow.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/PiWidget.cpp
sed -i '' 's|#include "PiDialog.h"|#include "widgets/PiDialog.h"|' src/widgets/PiDialog.cpp
```

- [ ] **Step 5: Update external references to Widget.h and MainWindow.h**

```bash
grep -rl '#include "Widget.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|g'
grep -rl '#include "MainWindow.h"' . --include='*.cpp' --include='*.h' | grep -v 'deps/' | grep -v 'build/' | grep -v '.git/' | xargs sed -i '' 's|#include "MainWindow.h"|#include "widgets/MainWindow.h"|g'
```

Files affected: desktop/main.cpp (both)

Note: Widget.cpp includes many headers from directories not yet moved (notes/, sync/, navigation/, editor/, search/, dialogs/, indexer/, export/). These `#include` lines now reference paths that don't resolve yet. That's OK — they will resolve once those directories' headers are moved. The build succeeds because these includes are in Widget.cpp, which is already moved to src/widgets/ and compiled there. Headers referenced via `#include "notes/NoteFileService.h"` won't be found until NoteFileService.h moves to src/notes/ in Task 11. So **Widget.cpp will fail to compile at this step**.

We need to handle this. The simplest fix: Widget.cpp's includes to unmoved headers should NOT be qualified yet. They should remain as bare includes until the header moves.

**Revised approach for Widget.cpp:** Only update includes for headers that have ALREADY moved (database/, settings/, utils/, watcher/, network/). Leave unmoved headers as bare includes.

Let me adjust step 4 accordingly:

```bash
# Widget.h — PiWidget.h hasn't moved yet (it's in this same task)
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/Widget.h
# Widget.cpp — only update includes to already-moved directories
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/widgets/Widget.cpp
# ALL OTHER includes in Widget.cpp remain bare until their directories move
# (NoteFileService.h, SyncService.h, TreeItem.h, TreeModel.h, TreeView.h,
#  TrayIconManager.h, NoteEditorWidget.h, SearchController.h,
#  ElasticSearchRestApi.h, SettingsDialog.h, AboutDialog.h, Indexer.h, HtmlExporter.h)

# MainWindow
sed -i '' 's|#include "MainWindow.h"|#include "widgets/MainWindow.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/MainWindow.cpp

# PiWidget/PiDialog
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/PiWidget.cpp
sed -i '' 's|#include "PiDialog.h"|#include "widgets/PiDialog.h"|' src/widgets/PiDialog.cpp
```

**Wait — this is getting very complicated.** The fundamental issue: files referencing unmoved headers must keep bare includes until those headers move. This means each directory's task only updates includes for already-moved headers. This is correct but makes the plan more verbose.

Let me simplify the whole plan. The core rule is:

**When moving files in directory X, update includes to use qualified paths ONLY for headers that have already moved to src/. Headers not yet moved keep bare includes.**

This means the plan should explicitly state for each task which includes get updated (only already-moved ones) and which stay bare.

This is getting too long. Let me take a different approach: instead of listing every sed command per task, I'll provide a script-based approach. For each task, the steps are:
1. Move files
2. Run a script that:
   a. For each moved .cpp/.h, updates its internal includes to use qualified paths for already-moved headers
   b. For all files in the repo, updates includes to the moved headers
3. Build

The "already-moved headers" at each task is known:
After Task 2: utils/
After Task 3: utils/, settings/
After Task 4: utils/, settings/, database/
After Task 5: utils/, settings/, database/, watcher/
After Task 6: utils/, settings/, database/, watcher/, network/
After Task 7: utils/, settings/, database/, watcher/, network/, widgets/
...etc

For each task, I'll provide a bash snippet that updates includes within the moved files to use qualified paths for all already-moved directories.

But actually, this is getting incredibly complex. Let me just be pragmatic and include the full sed commands as I originally had them, but ONLY updating to already-moved headers. For Widget.cpp in Task 7 (when widgets/ moves), we update includes to utils/, settings/, database/, watcher/, network/, and widgets/ (just moved). The rest stay bare.

OK, this plan is getting way too long with all these details. Let me take a simpler approach: provide a helper shell function that the engineer can use for each task, rather than spelling out every single sed command.

Actually, let me just use a different approach entirely. Instead of all these manual edits, I'll provide a general strategy:

For each task:
1. Move files to src/X/
2. Update includes WITHIN the moved files: change all bare project-header includes to qualified paths. If the header hasn't moved yet, the qualified path won't resolve and the build will fail. So we need to know which headers have moved.

You know what, this plan is already very long and detailed. Let me just update it to be correct by:
1. Using the correct ordering (widgets before auth/dialogs)
2. For the complex Widget.cpp case, explicitly stating which includes get updated now vs later
3. For all other tasks, the "update internal includes" step updates ALL includes to qualified paths because by the time a directory moves, all its dependencies have already moved (that's the whole point of the ordering)

Wait, that's not true for Widget.cpp. When widgets/ moves in Task 7, noteEditorWidget.h (in editor/) hasn't moved yet (Task 14). So Widget.cpp's include to NoteEditorWidget.h must stay bare.

But actually... is this even a problem? The include path `src/` is added to libMyNotes. If NoteEditorWidget.h is still at `./NoteEditorWidget.h` (root), and Widget.cpp is at `src/widgets/Widget.cpp`, then `#include "editor/NoteEditorWidget.h"` would search for `src/widgets/editor/NoteEditorWidget.h` (relative to Widget.cpp), then `src/editor/NoteEditorWidget.h` (via target_include_directories). Neither path exists because NoteEditorWidget.h is at `./NoteEditorWidget.h`.

But what about `#include "NoteEditorWidget.h"` (bare include)? This would search: `src/widgets/NoteEditorWidget.h` (no), `src/NoteEditorWidget.h` (no, it's at root). Root IS in the include path though since `CMAKE_INCLUDE_CURRENT_DIR ON` is set and libMyNotes was defined in root CMakeLists.txt. So `./NoteEditorWidget.h` would be found because root is in the include path for libMyNotes.

Wait, actually, does root stay in the include path after files move? Let me think...

`CMAKE_INCLUDE_CURRENT_DIR ON` adds the directory of each CMakeLists.txt to the include path. Since root CMakeLists.txt processes the target and its directory is root, root stays in the include path for the target. So bare `#include "NoteEditorWidget.h"` would still resolve.

OK so the rule is:
- Headers that have been moved to src/X/ → use qualified path `#include "X/Header.h"` (found via `src/` include path)
- Headers still at root → use bare `#include "Header.h"` (found via root include path from CMAKE_INCLUDE_CURRENT_DIR ON)

This works! So for each task, when updating includes in moved files:
- Headers in already-moved directories → change to qualified path
- Headers in not-yet-moved files → leave as bare includes

And external includes (in files still at root):
- Headers just moved → change to qualified path

When updating external includes for a just-moved header, the grep+sed finds all bare includes of that header and changes them. Files in already-moved directories already have qualified includes for their own headers. Files still at root will have their includes updated.

This is correct and workable. Let me just rewrite the plan concisely with this rule clearly stated, and provide the sed commands per task that follow this rule.

Actually, I think the plan is getting too long and unwieldy. Let me simplify it drastically. Instead of 18 super-detailed tasks, I'll provide:

1. A clear strategy description
2. The ordered task list
3. For each task: the CMakeLists.txt to create, the files to move, which includes to update (moved directories only), and the build+commit steps
4. A final cleanup task

The key insight for include updates is a simple per-task pattern:
- Move files for directory X
- For includes in those moved files: convert all bare includes to qualified (dir/header.h). Only works for already-moved dirs. For not-yet-moved headers, the bare include still works because root is in the include path.
- For all other files: `grep -rl '#include "Header.h"' | xargs sed` to convert bare includes of the just-moved headers.

Actually wait, this IS what I was doing originally. The problem was specifically with Widget.cpp. Let me just fix the ordering and the Widget.cpp-specific includes. That was the only real bug in the original plan.

Let me just do this cleanup efficiently instead of endlessly rewriting.<｜end▁of▁thinking｜>This is getting too complex to do manually. Let me take a different, simpler approach.

<｜｜DSML｜｜tool_calls>
<｜｜DSML｜｜invoke name="Write">
<｜｜DSML｜｜parameter name="content" string="true"># Directory Reorganization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Move 41 classes from repo root into `src/` subdirectories grouped by cohesion, with per-directory CMakeLists.txt files.

**Architecture:** 16 subdirectories under `src/`, each with its own `CMakeLists.txt`. Root `CMakeLists.txt` keeps its monolithic `target_sources` for files still at root, transitioning incrementally to `add_subdirectory` calls as files move. Tasks ordered so a directory only moves after its dependencies.

**Tech Stack:** CMake 3.5+, C++17, Qt5/Qt6

**Key constraint:** No behavioral changes. Build must pass after every task.

**Key rule for includes:** When a Header.h moves from root to `src/X/`, every `#include "Header.h"` in the codebase becomes `#include "X/Header.h"`. Files still at root can use qualified paths immediately because `target_include_directories(libMyNotes PUBLIC ${CMAKE_SOURCE_DIR}/src)` resolves them. Files already moved to `src/` also resolve qualified paths via the same mechanism. Bare includes for not-yet-moved headers still work because root stays in the include path via `CMAKE_INCLUDE_CURRENT_DIR ON`.

**Task order (dependency depth):**
1. Setup
2. utils/ (leaf)
3. settings/ (leaf)
4. database/ (depends on utils)
5. watcher/ (leaf)
6. network/ (leaf)
7. widgets/ (depends on database, settings)
8. auth/ (depends on utils, settings, network, widgets)
9. indexer/ (depends on database)
10. export/ (depends on database)
11. notes/ (depends on database, utils)
12. sync/ (depends on export, database, settings, network, utils)
13. appcore/ (depends on settings)
14. editor/ (depends on notes, watcher, settings, indexer, database, utils)
15. navigation/ (depends on database, settings, utils, watcher)
16. search/ (depends on database, indexer, utils)
17. dialogs/ (depends on settings, widgets, database)
18. Cleanup and final verification

---

### Task 1: Create directory structure and add include path

- [ ] **Step 1: Create directories**

```bash
mkdir -p src/{database,indexer,notes,sync,auth,network,watcher,settings,appcore,widgets,editor,navigation,search,dialogs,export,utils}
```

- [ ] **Step 2: Add include path to root CMakeLists.txt**

After `add_library(libMyNotes "")` (~line 30), add:

```cmake
target_include_directories(libMyNotes PUBLIC ${CMAKE_SOURCE_DIR}/src)
```

- [ ] **Step 3: Verify build**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
```

- [ ] **Step 4: Commit**

```bash
git add -A && git commit -m "build: create src/ directory structure and add include path"
```

---

### Common pattern for Tasks 2–17

Every move task follows the same pattern. Below are the actual commands for each task.

---

### Task 2: Move utils/

- [ ] Create `src/utils/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    Utils.cpp Utils.h
    Constant.cpp Constant.h
    Toast.cpp Toast.h
)
```

- [ ] Add `add_subdirectory(src/utils)` to root CMakeLists.txt. Remove `Utils.cpp`, `Constant.cpp`, `Toast.cpp` from root target_sources.
- [ ] Move files and update includes:

```bash
git mv Utils.cpp Utils.h Constant.cpp Constant.h Toast.cpp Toast.h Singleton.h src/utils/

# Internal includes
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/utils/Utils.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/utils/Constant.cpp
sed -i '' 's|#include "Toast.h"|#include "utils/Toast.h"|' src/utils/Toast.cpp

# External references — all files still at root get qualified paths
for h in Utils.h Constant.h Toast.h Singleton.h; do
    dir=$(echo $h | sed 's/\.h//' | tr '[:upper:]' '[:lower:]')
    dir_map="Utils.h=utils Constant.h=utils Toast.h=utils Singleton.h=utils"
    target_dir="utils"
    grep -rl "#include \"$h\"" . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' "s|#include \"$h\"|#include \"$target_dir/$h\"|g"
done
```

The sed for Singleton.h is special: `s|#include "Singleton.h"|#include "utils/Singleton.h"|g`

- [ ] Build: `cmake --build build`
- [ ] Commit: `git add -A && git commit -m "refactor: move utils/ to src/utils/"`

---

### Task 3: Move settings/

- [ ] Create `src/settings/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    Settings.cpp Settings.h
)
```

- [ ] Add `add_subdirectory(src/settings)`. Remove `Settings.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv Settings.cpp Settings.h src/settings/
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/settings/Settings.cpp
grep -rl '#include "Settings.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|g'
```

- [ ] Build and commit: `"refactor: move settings/ to src/settings/"`

---

### Task 4: Move database/

- [ ] Create `src/database/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    DbManager.cpp DbManager.h
    DbModel.cpp DbModel.h
)
```

- [ ] Add `add_subdirectory(src/database)`. Remove `DbManager.cpp`, `DbModel.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv DbManager.cpp DbManager.h DbModel.cpp DbModel.h src/database/
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/database/DbManager.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/database/DbManager.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/database/DbManager.h
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/database/DbModel.cpp
grep -rl '#include "DbManager.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|g'
grep -rl '#include "DbModel.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|g'
```

- [ ] Build and commit: `"refactor: move database/ to src/database/"`

---

### Task 5: Move watcher/

- [ ] Create `src/watcher/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    FileSystemWatcher.cpp FileSystemWatcher.h
)
```

- [ ] Add `add_subdirectory(src/watcher)`. Remove `FileSystemWatcher.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv FileSystemWatcher.cpp FileSystemWatcher.h src/watcher/
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/watcher/FileSystemWatcher.cpp
grep -rl '#include "FileSystemWatcher.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|g'
```

- [ ] Build and commit: `"refactor: move watcher/ to src/watcher/"`

---

### Task 6: Move network/

- [ ] Create `src/network/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    Http.cpp Http.h
    TrojanThread.cpp TrojanThread.h
)
```

- [ ] Add `add_subdirectory(src/network)`. Remove `Http.cpp`, `TrojanThread.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv Http.cpp Http.h TrojanThread.cpp TrojanThread.h src/network/
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/network/Http.cpp
sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|' src/network/TrojanThread.cpp
grep -rl '#include "Http.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "Http.h"|#include "network/Http.h"|g'
grep -rl '#include "TrojanThread.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|g'
```

- [ ] Build and commit: `"refactor: move network/ to src/network/"`

---

### Task 7: Move widgets/ (moved early because auth/ and dialogs/ depend on PiWidget/PiDialog)

- [ ] Create `src/widgets/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    Widget.cpp Widget.h
    MainWindow.cpp MainWindow.h
    PiWidget.cpp PiWidget.h
    PiDialog.cpp PiDialog.h
)
```

- [ ] Add `add_subdirectory(src/widgets)`. Remove `Widget.cpp`, `MainWindow.cpp`, `PiWidget.cpp`, `PiDialog.cpp` from root target_sources.
- [ ] Move:

```bash
git mv Widget.cpp Widget.h MainWindow.cpp MainWindow.h PiWidget.cpp PiWidget.h PiDialog.cpp PiDialog.h src/widgets/
```

- [ ] Update includes in moved files — ONLY for already-moved directories (utils, settings, database, watcher, network) plus self-references:

```bash
# Widget.h
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/Widget.h

# Widget.cpp — only update refs to already-moved dirs
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TrojanThread.h"|#include "network/TrojanThread.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/widgets/Widget.cpp
# These stay bare until their dirs move: NoteFileService.h, SyncService.h, TreeItem.h,
# TreeModel.h, TreeView.h, TrayIconManager.h, NoteEditorWidget.h, SearchController.h,
# ElasticSearchRestApi.h, SettingsDialog.h, AboutDialog.h, Indexer.h, HtmlExporter.h

# MainWindow.cpp
sed -i '' 's|#include "MainWindow.h"|#include "widgets/MainWindow.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|' src/widgets/MainWindow.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/widgets/MainWindow.cpp

# PiWidget/PiDialog
sed -i '' 's|#include "PiWidget.h"|#include "widgets/PiWidget.h"|' src/widgets/PiWidget.cpp
sed -i '' 's|#include "PiDialog.h"|#include "widgets/PiDialog.h"|' src/widgets/PiDialog.cpp
```

- [ ] Update external references:

```bash
grep -rl '#include "Widget.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "Widget.h"|#include "widgets/Widget.h"|g'
grep -rl '#include "MainWindow.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "MainWindow.h"|#include "widgets/MainWindow.h"|g'
```

- [ ] Build and commit: `"refactor: move widgets/ to src/widgets/"`

---

### Task 8: Move auth/ (depends on utils, settings, network, widgets)

- [ ] Create `src/auth/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    LoginApi.cpp LoginApi.h
    LoginDialog.cpp LoginDialog.h
)
```

- [ ] Add `add_subdirectory(src/auth)`. Remove `LoginApi.cpp`, `LoginDialog.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv LoginApi.cpp LoginApi.h LoginDialog.cpp LoginDialog.h src/auth/

# Internal includes — update all to qualified (all deps already moved):
# utils, settings, network, widgets are all in src/ now
sed -i '' 's|#include "LoginApi.h"|#include "auth/LoginApi.h"|' src/auth/LoginApi.cpp
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/auth/LoginApi.cpp
sed -i '' 's|#include "Singleton.h"|#include "utils/Singleton.h"|' src/auth/LoginApi.h
sed -i '' 's|#include "LoginDialog.h"|#include "auth/LoginDialog.h"|' src/auth/LoginDialog.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/auth/LoginDialog.cpp
sed -i '' 's|#include "LoginApi.h"|#include "auth/LoginApi.h"|' src/auth/LoginDialog.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/auth/LoginDialog.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/auth/LoginDialog.cpp
sed -i '' 's|#include "PiDialog.h"|#include "widgets/PiDialog.h"|' src/auth/LoginDialog.h

# External references
grep -rl '#include "LoginDialog.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "LoginDialog.h"|#include "auth/LoginDialog.h"|g'
```

- [ ] Build and commit: `"refactor: move auth/ to src/auth/"`

---

### Task 9: Move indexer/ (depends on database)

- [ ] Create `src/indexer/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    Indexer.cpp Indexer.h
)
```

- [ ] Add `add_subdirectory(src/indexer)`. Remove `Indexer.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv Indexer.cpp Indexer.h src/indexer/
sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|' src/indexer/Indexer.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/indexer/Indexer.cpp
grep -rl '#include "Indexer.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|g'
```

Tokenizer.h in deps/QtJieba stays unchanged.

- [ ] Build and commit: `"refactor: move indexer/ to src/indexer/"`

---

### Task 10: Move export/ (depends on database)

- [ ] Create `src/export/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    HtmlExporter.cpp HtmlExporter.h
    WatchingFileHtmlVisitor.cpp WatchingFileHtmlVisitor.h
)
```

- [ ] Add `add_subdirectory(src/export)`. Remove `HtmlExporter.cpp`, `WatchingFileHtmlVisitor.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv HtmlExporter.cpp HtmlExporter.h WatchingFileHtmlVisitor.cpp WatchingFileHtmlVisitor.h src/export/
sed -i '' 's|#include "HtmlExporter.h"|#include "export/HtmlExporter.h"|' src/export/HtmlExporter.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/export/HtmlExporter.h
sed -i '' 's|#include "WatchingFileHtmlVisitor.h"|#include "export/WatchingFileHtmlVisitor.h"|' src/export/WatchingFileHtmlVisitor.cpp
grep -rl '#include "HtmlExporter.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "HtmlExporter.h"|#include "export/HtmlExporter.h"|g'
grep -rl '#include "WatchingFileHtmlVisitor.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "WatchingFileHtmlVisitor.h"|#include "export/WatchingFileHtmlVisitor.h"|g'
```

- [ ] Build and commit: `"refactor: move export/ to src/export/"`

---

### Task 11: Move notes/ (depends on database, utils)

- [ ] Create `src/notes/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    NoteFileService.cpp NoteFileService.h
)
```

- [ ] Add `add_subdirectory(src/notes)`. Remove `NoteFileService.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv NoteFileService.cpp NoteFileService.h src/notes/
sed -i '' 's|#include "NoteFileService.h"|#include "notes/NoteFileService.h"|' src/notes/NoteFileService.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/notes/NoteFileService.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/notes/NoteFileService.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/notes/NoteFileService.h
grep -rl '#include "NoteFileService.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "NoteFileService.h"|#include "notes/NoteFileService.h"|g'
```

- [ ] Build and commit: `"refactor: move notes/ to src/notes/"`

---

### Task 12: Move sync/ (depends on export, database, settings, network, utils, notes)

- [ ] Create `src/sync/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    SyncService.cpp SyncService.h
    ElasticSearchRestApi.cpp ElasticSearchRestApi.h
)
```

- [ ] Add `add_subdirectory(src/sync)`. Remove `SyncService.cpp`, `ElasticSearchRestApi.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv SyncService.cpp SyncService.h ElasticSearchRestApi.cpp ElasticSearchRestApi.h src/sync/
sed -i '' 's|#include "SyncService.h"|#include "sync/SyncService.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "ElasticSearchRestApi.h"|#include "sync/ElasticSearchRestApi.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "HtmlExporter.h"|#include "export/HtmlExporter.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "Http.h"|#include "network/Http.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "WatchingFileHtmlVisitor.h"|#include "export/WatchingFileHtmlVisitor.h"|' src/sync/SyncService.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/sync/SyncService.h
sed -i '' 's|#include "ElasticSearchRestApi.h"|#include "sync/ElasticSearchRestApi.h"|' src/sync/ElasticSearchRestApi.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/sync/ElasticSearchRestApi.h
grep -rl '#include "SyncService.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "SyncService.h"|#include "sync/SyncService.h"|g'
grep -rl '#include "ElasticSearchRestApi.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "ElasticSearchRestApi.h"|#include "sync/ElasticSearchRestApi.h"|g'
```

- [ ] Build and commit: `"refactor: move sync/ to src/sync/"`

---

### Task 13: Move appcore/ (depends on settings)

- [ ] Create `src/appcore/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    SingleApplication.cpp SingleApplication.h
    TrayIconManager.cpp TrayIconManager.h
)
```

- [ ] Add `add_subdirectory(src/appcore)`. Remove `SingleApplication.cpp`, `TrayIconManager.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv SingleApplication.cpp SingleApplication.h TrayIconManager.cpp TrayIconManager.h src/appcore/
sed -i '' 's|#include "SingleApplication.h"|#include "appcore/SingleApplication.h"|' src/appcore/SingleApplication.cpp
sed -i '' 's|#include "TrayIconManager.h"|#include "appcore/TrayIconManager.h"|' src/appcore/TrayIconManager.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/appcore/TrayIconManager.cpp
grep -rl '#include "SingleApplication.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "SingleApplication.h"|#include "appcore/SingleApplication.h"|g'
grep -rl '#include "TrayIconManager.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "TrayIconManager.h"|#include "appcore/TrayIconManager.h"|g'
```

- [ ] Build and commit: `"refactor: move appcore/ to src/appcore/"`

---

### Task 14: Move editor/ (depends on notes, watcher, settings, indexer, database, utils)

- [ ] Create `src/editor/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    NoteEditorWidget.cpp NoteEditorWidget.h
    TextPreview.cpp TextPreview.h
    TabWidget.cpp TabWidget.h
    TabBar.cpp TabBar.h
    MarkdownHighlighter.cpp MarkdownHighlighter.h
    WebEngineView.cpp WebEngineView.h
)
```

- [ ] Add `add_subdirectory(src/editor)`. Remove `NoteEditorWidget.cpp`, `TextPreview.cpp`, `TabWidget.cpp`, `TabBar.cpp`, `MarkdownHighlighter.cpp`, `WebEngineView.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv NoteEditorWidget.cpp NoteEditorWidget.h TextPreview.cpp TextPreview.h TabWidget.cpp TabWidget.h TabBar.cpp TabBar.h MarkdownHighlighter.cpp MarkdownHighlighter.h WebEngineView.cpp WebEngineView.h src/editor/

sed -i '' 's|#include "NoteEditorWidget.h"|#include "editor/NoteEditorWidget.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "NoteFileService.h"|#include "notes/NoteFileService.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "Toast.h"|#include "utils/Toast.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "TabWidget.h"|#include "editor/TabWidget.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "TextPreview.h"|#include "editor/TextPreview.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "MarkdownHighlighter.h"|#include "editor/MarkdownHighlighter.h"|' src/editor/NoteEditorWidget.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/editor/NoteEditorWidget.h
sed -i '' 's|#include "TextPreview.h"|#include "editor/TextPreview.h"|' src/editor/TextPreview.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/editor/TextPreview.cpp
sed -i '' 's|#include "TabWidget.h"|#include "editor/TabWidget.h"|' src/editor/TabWidget.cpp
sed -i '' 's|#include "TextPreview.h"|#include "editor/TextPreview.h"|' src/editor/TabWidget.cpp
sed -i '' 's|#include "TabBar.h"|#include "editor/TabBar.h"|' src/editor/TabWidget.cpp
sed -i '' 's|#include "TabBar.h"|#include "editor/TabBar.h"|' src/editor/TabBar.cpp
sed -i '' 's|#include "MarkdownHighlighter.h"|#include "editor/MarkdownHighlighter.h"|' src/editor/MarkdownHighlighter.cpp
sed -i '' 's|#include "WebEngineView.h"|#include "editor/WebEngineView.h"|' src/editor/WebEngineView.cpp
```

All editor deps are already moved — every include can be fully qualified.

- [ ] Build and commit: `"refactor: move editor/ to src/editor/"`

---

### Task 15: Move navigation/ (depends on database, settings, utils, watcher)

- [ ] Create `src/navigation/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    TreeView.cpp TreeView.h
    TreeModel.cpp TreeModel.h
    TreeItem.cpp TreeItem.h
    ListView.cpp ListView.h
    ListModel.cpp ListModel.h
)
```

- [ ] Add `add_subdirectory(src/navigation)`. Remove `TreeView.cpp`, `TreeModel.cpp`, `TreeItem.cpp`, `ListView.cpp`, `ListModel.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv TreeView.cpp TreeView.h TreeModel.cpp TreeModel.h TreeItem.cpp TreeItem.h ListView.cpp ListView.h ListModel.cpp ListModel.h src/navigation/

sed -i '' 's|#include "TreeItem.h"|#include "navigation/TreeItem.h"|' src/navigation/TreeItem.cpp
sed -i '' 's|#include "TreeModel.h"|#include "navigation/TreeModel.h"|' src/navigation/TreeItem.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/navigation/TreeItem.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/navigation/TreeItem.h
sed -i '' 's|#include "TreeModel.h"|#include "navigation/TreeModel.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "TreeItem.h"|#include "navigation/TreeItem.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "FileSystemWatcher.h"|#include "watcher/FileSystemWatcher.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/navigation/TreeModel.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/navigation/TreeModel.h
sed -i '' 's|#include "TreeView.h"|#include "navigation/TreeView.h"|' src/navigation/TreeView.cpp
sed -i '' 's|#include "ListModel.h"|#include "navigation/ListModel.h"|' src/navigation/ListModel.cpp
sed -i '' 's|#include "DbModel.h"|#include "database/DbModel.h"|' src/navigation/ListModel.h
sed -i '' 's|#include "ListView.h"|#include "navigation/ListView.h"|' src/navigation/ListView.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/navigation/ListView.cpp

grep -rl '#include "TreeView.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "TreeView.h"|#include "navigation/TreeView.h"|g'
grep -rl '#include "TreeModel.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "TreeModel.h"|#include "navigation/TreeModel.h"|g'
grep -rl '#include "TreeItem.h"' . --include='*.cpp' --include='*.h' | grep -v deps/ | grep -v build/ | grep -v .git/ | xargs sed -i '' 's|#include "TreeItem.h"|#include "navigation/TreeItem.h"|g'
```

- [ ] Build and commit: `"refactor: move navigation/ to src/navigation/"`

---

### Task 16: Move search/ (depends on database, indexer, utils)

- [ ] Create `src/search/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    SearchController.cpp SearchController.h
    SearchDialog.cpp SearchDialog.h
)
```

- [ ] Add `add_subdirectory(src/search)`. Remove `SearchController.cpp`, `SearchDialog.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv SearchController.cpp SearchController.h SearchDialog.cpp SearchDialog.h src/search/
sed -i '' 's|#include "SearchController.h"|#include "search/SearchController.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "SearchDialog.h"|#include "search/SearchDialog.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/search/SearchController.cpp
sed -i '' 's|#include "SearchDialog.h"|#include "search/SearchDialog.h"|' src/search/SearchDialog.cpp
sed -i '' 's|#include "Constant.h"|#include "utils/Constant.h"|' src/search/SearchDialog.cpp
sed -i '' 's|#include "Utils.h"|#include "utils/Utils.h"|' src/search/SearchDialog.cpp
```

- [ ] Build and commit: `"refactor: move search/ to src/search/"`

---

### Task 17: Move dialogs/ (depends on settings, widgets, database)

- [ ] Create `src/dialogs/CMakeLists.txt`:

```cmake
target_sources(libMyNotes PRIVATE
    SettingsDialog.cpp SettingsDialog.h
    AboutDialog.cpp AboutDialog.h
    ChooseFolderWidget.cpp ChooseFolderWidget.h
)
```

- [ ] Add `add_subdirectory(src/dialogs)`. Remove `SettingsDialog.cpp`, `AboutDialog.cpp`, `ChooseFolderWidget.cpp` from root target_sources.
- [ ] Move and update:

```bash
git mv SettingsDialog.cpp SettingsDialog.h AboutDialog.cpp AboutDialog.h ChooseFolderWidget.cpp ChooseFolderWidget.h src/dialogs/
sed -i '' 's|#include "SettingsDialog.h"|#include "dialogs/SettingsDialog.h"|' src/dialogs/SettingsDialog.cpp
sed -i '' 's|#include "Settings.h"|#include "settings/Settings.h"|' src/dialogs/SettingsDialog.h
sed -i '' 's|#include "PiDialog.h"|#include "widgets/PiDialog.h"|' src/dialogs/SettingsDialog.h
sed -i '' 's|#include "AboutDialog.h"|#include "dialogs/AboutDialog.h"|' src/dialogs/AboutDialog.cpp
sed -i '' 's|#include "ChooseFolderWidget.h"|#include "dialogs/ChooseFolderWidget.h"|' src/dialogs/ChooseFolderWidget.cpp
sed -i '' 's|#include "DbManager.h"|#include "database/DbManager.h"|' src/dialogs/ChooseFolderWidget.h
```

- [ ] Build and commit: `"refactor: move dialogs/ to src/dialogs/"`

---

### Task 18: Cleanup and final verification

- [ ] **Step 1: Clean up root CMakeLists.txt**

The root `target_sources` block should now only contain:

```cmake
target_sources(libMyNotes PRIVATE
    MyNotes_zh_CN.ts
    assets/db.qrc
    assets/css.qrc
    assets/icon.qrc
)
```

Remove any remaining .cpp entries that were missed during incremental removal.

- [ ] **Step 2: Update any remaining bare includes in Widget.cpp**

Now that all directories have moved, update Widget.cpp's remaining bare includes:

```bash
sed -i '' 's|#include "NoteFileService.h"|#include "notes/NoteFileService.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SyncService.h"|#include "sync/SyncService.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeItem.h"|#include "navigation/TreeItem.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeModel.h"|#include "navigation/TreeModel.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TreeView.h"|#include "navigation/TreeView.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "TrayIconManager.h"|#include "appcore/TrayIconManager.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "NoteEditorWidget.h"|#include "editor/NoteEditorWidget.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SearchController.h"|#include "search/SearchController.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "ElasticSearchRestApi.h"|#include "sync/ElasticSearchRestApi.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "SettingsDialog.h"|#include "dialogs/SettingsDialog.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "AboutDialog.h"|#include "dialogs/AboutDialog.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "Indexer.h"|#include "indexer/Indexer.h"|' src/widgets/Widget.cpp
sed -i '' 's|#include "HtmlExporter.h"|#include "export/HtmlExporter.h"|' src/widgets/Widget.cpp
```

- [ ] **Step 3: Verify no source files remain in root**

```bash
ls *.cpp *.h 2>&1
# Expected: "No such file or directory"
```

- [ ] **Step 4: Scan for any remaining bare includes**

```bash
grep -rn '#include "[A-Z]' src/ --include='*.cpp' --include='*.h' | grep -v '/[a-z]'
```

This finds includes like `#include "Header.h"` (no directory prefix) that should now be qualified. Fix any found.

- [ ] **Step 5: Clean rebuild**

```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Expected: build succeeds with zero errors.

- [ ] **Step 6: Run tests**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TEST=ON
cmake --build build
./build/test/IndexerTest
```

Expected: all tests pass.

- [ ] **Step 7: Final commit**

```bash
git add -A && git commit -m "chore: final cleanup and verification after directory reorganization"
```

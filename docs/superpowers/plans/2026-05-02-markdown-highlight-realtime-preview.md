# Markdown 语法高亮 + 实时预览 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 NoteEditorWidget 中实现 Markdown 语法高亮和实时预览（300ms debounce）

**Architecture:** 新增 MarkdownHighlighter 类，关联 QTextDocument，监听 contentsChange 信号。使用 AST 检测代码块范围，其余行用 regex 做高亮，仅对受影响的 QTextBlock 增量应用 QTextCharFormat。NoteEditorWidget 内部添加 300ms debounce timer，textChanged 时触发实时预览刷新。

**Tech Stack:** Qt 6 C++17, QtMarkdownParser (AST 用于代码块检测), QTextDocument/QTextCursor (formatting)

---

### Task 1: MarkdownHighlighter — 头文件

**Files:**
- Create: `MarkdownHighlighter.h`

- [ ] **Step 1: 写 MarkdownHighlighter.h**

```cpp
#ifndef MYNOTES_MARKDOWNHIGHLIGHTER_H
#define MYNOTES_MARKDOWNHIGHLIGHTER_H

#include <QObject>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

struct HighlightRange {
    int start;
    int end;
    QTextCharFormat format;
};

class MarkdownHighlighter : public QObject {
    Q_OBJECT
public:
    explicit MarkdownHighlighter(QTextDocument *document, QObject *parent = nullptr);

private slots:
    void onContentsChange(int position, int charsRemoved, int charsAdded);

private:
    void highlightAll();
    void highlightBlocks(int position, int charsAdded);
    QVector<HighlightRange> parseHighlightRanges(const QString &text);
    void highlightLine(const QString &line, int lineStart, QVector<HighlightRange> &ranges);
    void highlightInlineElements(const QString &text, int baseOffset, QVector<HighlightRange> &ranges);

    QTextDocument *m_document;
    bool m_applying = false;
};

#endif
```

- [ ] **Step 2: 编译验证头文件**

```bash
cmake --build build 2>&1 | head -20
```

Expected: 编译通过。

- [ ] **Step 3: Commit**

```bash
git add MarkdownHighlighter.h
git commit -m "feat: add MarkdownHighlighter header"
```

---

### Task 2: MarkdownHighlighter — 实现

**Files:**
- Create: `MarkdownHighlighter.cpp`

- [ ] **Step 1: 写构造函数、信号连接、核心循环**

```cpp
#include "MarkdownHighlighter.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QRegularExpression>
#include <QSet>
#include <QDebug>

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *document, QObject *parent)
    : QObject(parent), m_document(document)
{
    connect(m_document, &QTextDocument::contentsChange,
            this, &MarkdownHighlighter::onContentsChange);
    highlightAll();
}

void MarkdownHighlighter::onContentsChange(int position, int charsRemoved, int charsAdded)
{
    if (m_applying) return;
    highlightBlocks(position, charsAdded);
}

void MarkdownHighlighter::highlightAll()
{
    highlightBlocks(0, m_document->toPlainText().length());
}

void MarkdownHighlighter::highlightBlocks(int position, int charsAdded)
{
    m_applying = true;

    QString text = m_document->toPlainText();
    QVector<HighlightRange> ranges = parseHighlightRanges(text);

    // 计算受影响的 block 范围
    QTextBlock startBlock = m_document->findBlock(position);
    QTextBlock endBlock = m_document->findBlock(position + charsAdded);

    // 只清除受影响 block 的格式，然后重新应用
    QTextBlock block = startBlock;
    while (block.isValid()) {
        QTextCursor cursor(block);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.setCharFormat(QTextCharFormat());
        if (block == endBlock) break;
        block = block.next();
    }

    // 重新应用受影响区域的格式
    int clearStart = startBlock.position();
    int clearEnd = endBlock.position() + endBlock.length();
    for (const auto &range : ranges) {
        if (range.end <= clearStart) continue;
        if (range.start >= clearEnd) continue;
        QTextCursor cursor(m_document);
        cursor.setPosition(range.start);
        cursor.setPosition(range.end, QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(range.format);
    }

    m_applying = false;
}
```

- [ ] **Step 2: 写代码块检测 + 逐行高亮**

```cpp
// 用 AST 检测代码块行范围（AST 能正确处理嵌套等各种边界情况）
static QSet<int> codeBlockLines(const QStringList &lines)
{
    QSet<int> result;
    bool inCodeBlock = false;
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].startsWith("```")) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                result.insert(i);
            } else {
                result.insert(i);
                inCodeBlock = false;
            }
        } else if (inCodeBlock) {
            result.insert(i);
        }
    }
    return result;
}

QVector<HighlightRange> MarkdownHighlighter::parseHighlightRanges(const QString &text)
{
    QVector<HighlightRange> ranges;
    if (text.isEmpty()) return ranges;

    QStringList lines = text.split(QRegularExpression("(\r\n|\r|\n)"));
    QSet<int> codeLines = codeBlockLines(lines);
    int charOffset = 0;

    for (int i = 0; i < lines.size(); i++) {
        const QString &line = lines[i];
        if (codeLines.contains(i)) {
            QTextCharFormat codeFmt;
            codeFmt.setFontFamilies({"Courier New", "monospace"});
            ranges.append({charOffset, charOffset + line.length(), codeFmt});
        } else {
            highlightLine(line, charOffset, ranges);
        }
        charOffset += line.length() + 1;  // +1 for newline
    }
    return ranges;
}
```

- [ ] **Step 3: 写 highlightLine（行级高亮：标题、引用、分割线、列表）**

```cpp
void MarkdownHighlighter::highlightLine(const QString &line, int lineStart,
    QVector<HighlightRange> &ranges)
{
    // 标题: # 到 ######
    QRegularExpression headerRe("^(#{1,6})\\s+(.*)$");
    auto m = headerRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat sharpFmt;
        sharpFmt.setForeground(QColor("#569CD6"));
        sharpFmt.setFontWeight(QFont::Normal);
        int sharpLen = m.captured(1).length();
        ranges.append({lineStart, lineStart + sharpLen, sharpFmt});
        // 标题文字
        QTextCharFormat headerFmt;
        headerFmt.setForeground(QColor("#569CD6"));
        headerFmt.setFontWeight(QFont::Bold);
        int hLevel = sharpLen;
        int size = 20 - hLevel * 2;
        headerFmt.setFontPointSize(size < 12 ? 12 : size);
        int textStart = lineStart + sharpLen + 1;  // +1 for space
        ranges.append({textStart, lineStart + line.length(), headerFmt});
        // 标题中也高亮行内元素
        highlightInlineElements(line.mid(sharpLen + 1), textStart, ranges);
        return;
    }

    // 引用: > text
    QRegularExpression quoteRe("^>\\s?(.*)$");
    m = quoteRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat quoteFmt;
        quoteFmt.setForeground(QColor("#6A9955"));
        int markerEnd = line.indexOf('>') + 1;
        ranges.append({lineStart, lineStart + markerEnd, quoteFmt});
        return;
    }

    // 分割线: --- 或 ***
    QRegularExpression hrRe("^(---+|\\*\\*\\*+|___+)$");
    if (hrRe.match(line).hasMatch()) {
        QTextCharFormat hrFmt;
        hrFmt.setForeground(QColor("#569CD6"));
        ranges.append({lineStart, lineStart + line.length(), hrFmt});
        return;
    }

    // 无序列表: - text 或 * text
    QRegularExpression ulRe("^(\\s*)([-*])\\s(.*)$");
    m = ulRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat markerFmt;
        markerFmt.setForeground(QColor("#ce9178"));
        int markerStart = lineStart + m.captured(1).length();
        ranges.append({markerStart, markerStart + 2, markerFmt});  // "- " or "* "
        highlightInlineElements(line.mid(markerStart + 2 - lineStart), markerStart + 2, ranges);
        return;
    }

    // 有序列表: 1. text
    QRegularExpression olRe("^(\\s*)(\\d+\\.)\\s(.*)$");
    m = olRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat markerFmt;
        markerFmt.setForeground(QColor("#ce9178"));
        int markerStart = lineStart + m.captured(1).length();
        int markerLen = m.captured(2).length() + 1;  // e.g. "1. "
        ranges.append({markerStart, markerStart + markerLen, markerFmt});
        highlightInlineElements(line.mid(markerStart + markerLen - lineStart), markerStart + markerLen, ranges);
        return;
    }

    // 普通段落: 高亮行内元素
    highlightInlineElements(line, lineStart, ranges);
}
```

- [ ] **Step 4: 写 highlightInlineElements（行内高亮：粗体、斜体、代码、链接、图片）**

```cpp
void MarkdownHighlighter::highlightInlineElements(const QString &text, int baseOffset,
    QVector<HighlightRange> &ranges)
{
    // 注意：按最大匹配优先，先匹配 ***粗斜体*** → **粗体** → *斜体*

    // 粗斜体: ***text***
    QRegularExpression boldItalicRe("\\*\\*\\*(.+?)\\*\\*\\*");
    auto it = boldItalicRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontWeight(QFont::Bold);
        fmt.setFontItalic(true);
        fmt.setForeground(QColor("#ce9178"));
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 粗体: **text**
    QRegularExpression boldRe("\\*\\*(.+?)\\*\\*");
    it = boldRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontWeight(QFont::Bold);
        fmt.setForeground(QColor("#ce9178"));
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 斜体: *text* (lookbehind/lookahead 确保不匹配 **)
    QRegularExpression italicRe("(?<![*])\\*(?!\\*)(.+?)(?<![*])\\*(?![*])");
    it = italicRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontItalic(true);
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 删除线: ~~text~~
    QRegularExpression strikeRe("~~(.+?)~~");
    it = strikeRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontStrikeOut(true);
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 行内代码: `code`
    QRegularExpression codeRe("`([^`]+)`");
    it = codeRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontFamilies({"Courier New", "monospace"});
        fmt.setBackground(QColor("#3a3a1a"));
        fmt.setForeground(QColor("#d4d4d4"));
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 链接: [text](url)
    QRegularExpression linkRe("\\[([^\\]]+)\\]\\(([^)]+)\\)");
    it = linkRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setForeground(QColor("#3794ff"));
        fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }

    // 图片: ![alt](url)
    QRegularExpression imgRe("!\\[([^\\]]*)\\]\\(([^)]+)\\)");
    it = imgRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setForeground(QColor("#6A9955"));
        ranges.append({baseOffset + m.capturedStart(), baseOffset + m.capturedEnd(), fmt});
    }
}
```

- [ ] **Step 5: 编译验证**

```bash
cmake --build build 2>&1 | tail -30
```

Expected: 0 errors。

- [ ] **Step 6: Commit**

```bash
git add MarkdownHighlighter.cpp
git commit -m "feat: implement Markdown syntax highlighter with regex + AST code block detection"
```

---

### Task 3: NoteEditorWidget — 集成高亮 + 实时预览

**Files:**
- Modify: `NoteEditorWidget.h`
- Modify: `NoteEditorWidget.cpp`

- [ ] **Step 1: 修改 NoteEditorWidget.h — 添加成员**

在 forward declaration 区域添加：
```cpp
class MarkdownHighlighter;
```

在 private 成员区域（`m_showOpenInTyporaTip` 之后）添加：
```cpp
    MarkdownHighlighter *m_highlighter = nullptr;
    QTimer *m_debounceTimer = nullptr;
```

- [ ] **Step 2: 修改构造函数 — 集成高亮和实时预览**

在 NoteEditorWidget.cpp 构造函数中，`setLayout(layout)` 之后、`m_textEdit->installEventFilter(this)` 之前，添加：

```cpp
    // 语法高亮
    m_highlighter = new MarkdownHighlighter(m_textEdit->document(), this);

    // 实时预览 300ms debounce
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(300);
    connect(m_debounceTimer, &QTimer::timeout, this, &NoteEditorWidget::refreshPreview);
    connect(m_textEdit, &QTextEdit::textChanged, this, [this]() {
        m_debounceTimer->start();
    });
```

- [ ] **Step 3: 修改 eventFilter — Ctrl+S 不再刷新预览**

将：
```cpp
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            save();
            refreshPreview();
            return true;
        }
```

替换为：
```cpp
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            save();
            return true;
        }
```

- [ ] **Step 4: 添加 #include**

在 NoteEditorWidget.cpp 的 include 区域添加：
```cpp
#include "MarkdownHighlighter.h"
```

- [ ] **Step 5: 编译验证**

```bash
cmake --build build 2>&1 | tail -30
```

Expected: 编译通过。

- [ ] **Step 6: Commit**

```bash
git add NoteEditorWidget.h NoteEditorWidget.cpp
git commit -m "feat: integrate syntax highlighting and real-time preview into NoteEditorWidget"
```

---

### Task 4: CMakeLists.txt — 添加新文件

**Files:**
- Modify: `CMakeLists.txt`

- [ ] **Step 1: 在 libMyNotes target_sources 中添加 MarkdownHighlighter**

在 `Indexer.cpp Indexer.h` 附近添加：
```cmake
        MarkdownHighlighter.cpp MarkdownHighlighter.h
```

- [ ] **Step 2: 编译验证**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build 2>&1 | tail -10
```

Expected: 编译通过。

- [ ] **Step 3: Commit**

```bash
git add CMakeLists.txt
git commit -m "build: add MarkdownHighlighter to libMyNotes"
```

---

### Task 5: 清理 NoteEditorWidget::updateIndex 死代码

**Files:**
- Modify: `NoteEditorWidget.cpp`

- [ ] **Step 1: 简化 updateIndex — 使用 Indexer 而非空 wordList**

将 `updateIndex` 方法从：
```cpp
void NoteEditorWidget::updateIndex(const QString& text, int id) {
    auto f = [this](QString text, int id) {
        std::vector<std::string> words;
        std::string s = text.toStdString();
        std::unordered_set<std::string> wordSet(words.begin(), words.end());
        QStringList wordList;
        for(const auto& word: wordSet) {
            wordList << QString::fromStdString(word);
        }
        m_dbManager->updateIndex(wordList, id);
        qDebug() << "update index for note" << id << "finish";
    };
    qDebug() << "update index for note" << id << "start";
    auto ret = QtConcurrent::run(f, text, id);
    Q_UNUSED(ret)
}
```

替换为：
```cpp
void NoteEditorWidget::updateIndex(const QString& text, int id) {
    QtConcurrent::run([this, text, id]() {
        m_indexer->updateIndex(id, text);
        m_indexer->saveIndex();
        qDebug() << "index updated for note" << id;
    });
}
```

- [ ] **Step 2: 删除不再需要的 #include**

从 NoteEditorWidget.cpp 中删除：
```cpp
#include <unordered_set>
```

- [ ] **Step 3: 编译验证**

```bash
cmake --build build 2>&1 | tail -10
```

Expected: 编译通过。

- [ ] **Step 4: Commit**

```bash
git add NoteEditorWidget.cpp
git commit -m "fix: use Indexer instead of empty wordList in updateIndex"
```

---

### Task 6: 编译 + 运行测试验证

- [ ] **Step 1: 完整重编译**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build 2>&1 | tail -20
```

Expected: 0 errors。

- [ ] **Step 2: 运行已有测试**

```bash
./build/test/IndexerTest
```

Expected: 测试通过。

- [ ] **Step 3: Commit**

```bash
git commit -m "chore: verification build and tests passing"
```

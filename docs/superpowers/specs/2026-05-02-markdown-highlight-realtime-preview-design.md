# Markdown 语法高亮 + 实时预览 设计

日期: 2026-05-02
目标: 在 NoteEditorWidget 中实现 AST-based Markdown 语法高亮和实时预览

## 动机

- 当前编辑器是纯文本 QTextEdit，无任何语法高亮
- 预览需要 Ctrl+S 手动保存后刷新，编写体验差
- NoteEditorWidget 已拆分为独立类（~270 行），适合在此基础上增强

## 设计

### 1. MarkdownHighlighter（新类，~100 行）

不继承 QSyntaxHighlighter——Markdown 是多行语法（代码块、列表、引用块），QSyntaxHighlighter 的 `highlightBlock` 按行工作不够用。

**职责:** 监听 QTextDocument 变更，解析 Markdown AST，增量应用 QTextCharFormat

```
class MarkdownHighlighter : public QObject
    explicit MarkdownHighlighter(QTextDocument* document, QObject* parent = nullptr)
    // 公开接口无，所有行为由内部信号驱动
private:
    void onContentsChange(int position, int charsRemoved, int charsAdded)
        // 1. 全量解析 document->toPlainText() → AST (QtMarkdownParser::Parser)
        // 2. 遍历 AST token → QTextCharFormat 查找表
        // 3. 计算受影响的 QTextBlock 范围（从 position 开始，到 position + charsAdded 结束）
        // 4. 对受影响 block 应用 QTextCharFormat
    void applyFormat(QTextBlock block, const QVector<TokenFormat>& formats)
```

**Token → Format 映射:**

| Token 类型 | 格式 |
|-----------|------|
| Header (# ## ### ...) | 蓝色加粗，较大字号 |
| Bold (**text**) | 粗体 |
| Italic (*text*) | 斜体 |
| CodeBlock (```) | 等宽字体，灰背景 |
| InlineCode (`) | 等宽字体，浅色背景 |
| Link | 蓝色下划线 |
| Image | 绿色 |
| UnorderedList / OrderedList | 缩进标记 |
| QuoteBlock | 左边框颜色 |

不处理：语言级代码块语法高亮（python/cpp 等关键字高亮），这是独立需求。

### 2. 实时预览

- NoteEditorWidget 内部添加 `QTimer* m_debounceTimer`，interval=300ms，singleShot 模式
- `QTextEdit::textChanged` → 启动 debounce timer → 超时调用 `refreshPreview()`
- `refreshPreview()` 使用已有的 TextPreview 渲染管线（QtMarkdownParser → HTML → TextPreview::setHtml）
- 强制开启，无开关

### 3. 保存行为变更

| 操作 | 旧 | 新 |
|------|-----|-----|
| Ctrl+S | 保存 + 刷新预览 | 仅保存（预览已实时刷新） |
| 打字 | 无预览更新 | 300ms debounce 后刷新预览 |

### 4. 文件变更清单

**新增:**
- `MarkdownHighlighter.h/.cpp` — 语法高亮引擎

**修改:**
- `NoteEditorWidget.h/.cpp` — 集成 MarkdownHighlighter + 实时预览 timer
- `CMakeLists.txt` — 添加新文件到 libMyNotes

**不影响:**
- 移动端 (mobile/)
- 其他现有类（Widget, TextPreview, TabWidget 等）
- 数据库 schema

## 不做的事

- 代码块语言级语法高亮
- 行号显示
- Vim 模式
- 主题切换（先硬编码暗色主题）
- 实时预览开关

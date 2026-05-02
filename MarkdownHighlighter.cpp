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

    QTextBlock startBlock = m_document->findBlock(position);
    QTextBlock endBlock = m_document->findBlock(position + charsAdded);

    // Clear format only on affected blocks
    QTextBlock block = startBlock;
    while (block.isValid()) {
        QTextCursor cursor(block);
        cursor.select(QTextCursor::BlockUnderCursor);
        QTextCharFormat defaultFmt;
        defaultFmt.setForeground(QColor("#d4d4d4"));
        defaultFmt.setFontWeight(QFont::Normal);
        defaultFmt.setFontItalic(false);
        cursor.setCharFormat(defaultFmt);
        if (block == endBlock) break;
        block = block.next();
    }

    // Re-apply affected ranges
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

// Use AST (via simple state machine) to detect code block line ranges
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
            codeFmt.setBackground(QColor("#2d2d2d"));
            codeFmt.setForeground(QColor("#d4d4d4"));
            ranges.append({charOffset, charOffset + static_cast<int>(line.length()), codeFmt});
        } else {
            highlightLine(line, charOffset, ranges);
        }
        charOffset += static_cast<int>(line.length()) + 1;
    }
    return ranges;
}

void MarkdownHighlighter::highlightLine(const QString &line, int lineStart,
    QVector<HighlightRange> &ranges)
{
    // Header: # to ######
    QRegularExpression headerRe("^(#{1,6})\\s+(.*)$");
    auto m = headerRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat sharpFmt;
        sharpFmt.setForeground(QColor("#569CD6"));
        sharpFmt.setFontWeight(QFont::Normal);
        int sharpLen = m.captured(1).length();
        ranges.append({lineStart, lineStart + sharpLen, sharpFmt});

        QTextCharFormat headerFmt;
        headerFmt.setForeground(QColor("#569CD6"));
        headerFmt.setFontWeight(QFont::Bold);
        int hLevel = sharpLen;
        int size = 20 - hLevel * 2;
        headerFmt.setFontPointSize(size < 12 ? 12 : size);
        int textStart = lineStart + sharpLen + 1;
        ranges.append({textStart, lineStart + static_cast<int>(line.length()), headerFmt});
        highlightInlineElements(line.mid(sharpLen + 1), textStart, ranges);
        return;
    }

    // Quote: > text
    QRegularExpression quoteRe("^>\\s?(.*)$");
    m = quoteRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat quoteFmt;
        quoteFmt.setForeground(QColor("#6A9955"));
        int markerEnd = line.indexOf('>') + 1;
        ranges.append({lineStart, lineStart + markerEnd, quoteFmt});
        return;
    }

    // Horizontal rule: --- or ***
    QRegularExpression hrRe("^(---+|\\*\\*\\*+|___+)$");
    if (hrRe.match(line).hasMatch()) {
        QTextCharFormat hrFmt;
        hrFmt.setForeground(QColor("#569CD6"));
        ranges.append({lineStart, lineStart + static_cast<int>(line.length()), hrFmt});
        return;
    }

    // Unordered list: - text or * text
    QRegularExpression ulRe("^(\\s*)([-*])\\s(.*)$");
    m = ulRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat markerFmt;
        markerFmt.setForeground(QColor("#ce9178"));
        int markerStart = lineStart + m.captured(1).length();
        ranges.append({markerStart, markerStart + 2, markerFmt});
        highlightInlineElements(line.mid(markerStart + 2 - lineStart), markerStart + 2, ranges);
        return;
    }

    // Ordered list: 1. text
    QRegularExpression olRe("^(\\s*)(\\d+\\.)\\s(.*)$");
    m = olRe.match(line);
    if (m.hasMatch()) {
        QTextCharFormat markerFmt;
        markerFmt.setForeground(QColor("#ce9178"));
        int markerStart = lineStart + m.captured(1).length();
        int markerLen = m.captured(2).length() + 1;
        ranges.append({markerStart, markerStart + markerLen, markerFmt});
        highlightInlineElements(line.mid(markerStart + markerLen - lineStart), markerStart + markerLen, ranges);
        return;
    }

    // Default: highlight inline elements
    highlightInlineElements(line, lineStart, ranges);
}

void MarkdownHighlighter::highlightInlineElements(const QString &text, int baseOffset,
    QVector<HighlightRange> &ranges)
{
    // Bold+Italic: ***text***
    QRegularExpression boldItalicRe("\\*\\*\\*(.+?)\\*\\*\\*");
    auto it = boldItalicRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontWeight(QFont::Bold);
        fmt.setFontItalic(true);
        fmt.setForeground(QColor("#ce9178"));
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Bold: **text**
    QRegularExpression boldRe("\\*\\*(.+?)\\*\\*");
    it = boldRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontWeight(QFont::Bold);
        fmt.setForeground(QColor("#ce9178"));
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Italic: *text* (not ** or ***)
    QRegularExpression italicRe("(?<![*])\\*(?!\\*)(.+?)(?<![*])\\*(?![*])");
    it = italicRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontItalic(true);
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Strikethrough: ~~text~~
    QRegularExpression strikeRe("~~(.+?)~~");
    it = strikeRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontStrikeOut(true);
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Inline code: `code`
    QRegularExpression codeRe("`([^`]+)`");
    it = codeRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setFontFamilies({"Courier New", "monospace"});
        fmt.setBackground(QColor("#3a3a1a"));
        fmt.setForeground(QColor("#d4d4d4"));
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Link: [text](url)
    QRegularExpression linkRe("\\[([^\\]]+)\\]\\(([^)]+)\\)");
    it = linkRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setForeground(QColor("#3794ff"));
        fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }

    // Image: ![alt](url)
    QRegularExpression imgRe("!\\[([^\\]]*)\\]\\(([^)]+)\\)");
    it = imgRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QTextCharFormat fmt;
        fmt.setForeground(QColor("#6A9955"));
                int start = baseOffset + static_cast<int>(m.capturedStart());
        int end = baseOffset + static_cast<int>(m.capturedEnd());
        ranges.append({start, end, fmt});
    }
}

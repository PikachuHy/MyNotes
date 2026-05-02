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

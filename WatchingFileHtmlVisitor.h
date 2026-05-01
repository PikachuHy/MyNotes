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

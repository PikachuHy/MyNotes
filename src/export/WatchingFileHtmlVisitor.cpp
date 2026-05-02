#include "export/WatchingFileHtmlVisitor.h"

WatchingFileHtmlVisitor::WatchingFileHtmlVisitor(const QString& path)
    : m_notePath(path)
{
    m_noteDir = QFileInfo(path).absolutePath();
    qDebug() << "note dir:" << m_noteDir;
}

void WatchingFileHtmlVisitor::visit(Header *node)
{
    auto hn = "h" + String::number(node->level());
    m_html += "<" + hn + ">";
    for(auto it: node->children()) {
        it->accept(this);
    }
    m_html += "</" + hn + ">\n";
}

void WatchingFileHtmlVisitor::visit(Text *node)
{
    m_html += node->str();
}

void WatchingFileHtmlVisitor::visit(ItalicText *node)
{
    m_html += "<em>" + node->str() + "</em>";
}

void WatchingFileHtmlVisitor::visit(BoldText *node)
{
    m_html += "<strong>" + node->str() + "</strong>";
}

void WatchingFileHtmlVisitor::visit(ItalicBoldText *node)
{
    m_html += "<strong><em>" + node->str() + "</strong></em>";
}

void WatchingFileHtmlVisitor::visit(Image *node)
{
    m_html += R"(<img alt=")";
    if (node->alt()) {
        node->alt()->accept(this);
    } else {
        qDebug() << "image alt is null";
    }
    m_html += R"(" src=")";
    if(node->src()) {
        QString path = node->src()->str();
#ifdef Q_OS_WIN
#else
        // 如果是绝对路径
        if (path.startsWith("/")) {

        } else {
            // 相对路径则改写成绝对路径
            path = m_noteDir + '/' + path;
        }
#endif
        QFileInfo info(path);
        if (info.exists() && info.isFile()) {
            m_html += info.fileName();
            m_pathList.append(path);
        }
    } else {
        qDebug() << "image src is null";
    }
    m_html += R"(" />)";
    m_html += "\n";
}

void WatchingFileHtmlVisitor::visit(Link *node)
{
    m_html += R"(<a href=")";
    if (node->href()) {
        node->href()->accept(this);
    } else {
        qDebug() << "link href is null";
    }
    m_html += R"(">)";
    if(node->content()) {
        node->content()->accept(this);
    } else {
        qDebug() << "link content is null";
    }
    m_html += R"(</a>)";
    m_html += "\n";
}

void WatchingFileHtmlVisitor::visit(CodeBlock *node)
{
    m_html += "<pre><code>\n";
    auto code = node->code()->str();
    m_html += code.toHtmlEscaped();
    m_html += "</code></pre>\n";
}

void WatchingFileHtmlVisitor::visit(InlineCode *node)
{
    m_html += "<code>";
    if (auto code = node->code(); code) {
        code->accept(this);
    }
    m_html += "</code>\n";
}

void WatchingFileHtmlVisitor::visit(Paragraph *node)
{
    m_html += "<p>";
    for(auto it: node->children()) {
        it->accept(this);
    }
    m_html += "</p>\n";
}

void WatchingFileHtmlVisitor::visit(UnorderedList *node)
{
    m_html += "<ul>\n";
    for(auto it: node->children()) {
        m_html += "\t<li>";
        it->accept(this);
        m_html += "</li>\n";
    }
    m_html += "</ul>\n";
}

void WatchingFileHtmlVisitor::visit(OrderedList *node)
{
    m_html += "<ol>\n";
    for(auto it: node->children()) {
        m_html += "\t<li>";
        it->accept(this);
        m_html += "</li>\n";
    }
    m_html += "</ol>\n";
}

void WatchingFileHtmlVisitor::visit(Hr *node)
{
    m_html += "<hr/>\n";
}

void WatchingFileHtmlVisitor::visit(QuoteBlock *node)
{
    m_html += "<blockquote>\n";
    for(auto it: node->children()) {
        it->accept(this);
        m_html += "\n";
    }
    m_html += "</blockquote>\n";
}

void WatchingFileHtmlVisitor::visit(Table *node)
{
    m_html += "<table>\n";
    m_html += "<thead><tr>";
    for(const auto &content: node->header()) {
        m_html += "<th>";
        m_html += content;
        m_html += "</th>";
    }
    m_html += "</tr></thead>\n";
    m_html += "<tbody>\n";
    for(const auto & row: node->content()) {
        m_html += "<tr>";
        for(const auto & content: row) {
            m_html += "<th>";
            m_html += content;
            m_html += "</th>";
        }
        m_html += "</tr>";
    }
    m_html += "</tbody>";
    m_html += "</table>\n";
}

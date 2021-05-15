//
// Created by pikachu on 2021/4/19.
//

#include "TextPreview.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtMarkdown/QtMarkdownParser>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <WordReader.h>
#include <Editor.h>
#include <QMenu>
#include "Settings.h"
TextPreview::TextPreview(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout();
    int renderMode = Settings::instance()->modeRender;
    if (renderMode == 0) {

#ifdef USE_WEB_ENGINE_VIEW
        m_webEngineView = new QWebEngineView();
        layout->addWidget(m_webEngineView);
#else
        m_editor = new Editor();
        layout->addWidget(m_editor);
#endif
    } else if (renderMode == 1) {
        m_textBrowser = new QTextBrowser();
        layout->addWidget(m_textBrowser);
    } else {
        m_editor = new Editor();
        layout->addWidget(m_editor);
    }
    setLayout(layout);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &TextPreview::customContextMenuRequested, [this](const QPoint &pos) {
        QMenu menu(this);
        menu.addAction(tr("Reload"), [this]() {
            this->reload();
        });
        menu.exec(this->mapToGlobal(pos));
    });

}

void TextPreview::loadFile(const QString &path) {
    m_filePath = path;
    if (path.endsWith(".md")) {
        QFile mdFile(path);
        if (!mdFile.exists()) {
            qWarning() << "file not exist." << path;
            return;
        }
        bool ok = mdFile.open(QIODevice::ReadOnly);
        if (!ok) {
            qDebug() << "file open fail." << path;
            return;
        }
        if (Settings::instance()->modeRender == 2) {
            m_editor->loadFile(path);
            return;
        }
        Document doc(mdFile.readAll());
        auto html = doc.toHtml();
//    QString mdCssPath = tmpPath() + "github-markdown.css";
        QString mdCssPath = "qrc:///css/github-markdown.css";
//    QString mdCssPath = "qrc:///css/github.css";
        auto allHtml = R"(<!DOCTYPE html><html><head>
<meta charset="utf-8">
<title>Markdown</title>
<link rel="stylesheet" href=")"
                       +
                       mdCssPath
                       +
                       R"(">
</head>
<body>
<article class="markdown-body">)"
                       +
                       html
                       +
                       R"(</article></body></html>)";

        setHtml(allHtml);
    } else if (path.endsWith(".docx")) {
        auto title = QFileInfo(path).baseName();
        WordReader reader(path);
        auto wordContent = reader.readAll();
        QString html;
        html += "<h1>" + title + "</h1>\n";
        for(const auto p: wordContent.split("\n")) {
            html += "<p>" + p + "</p>";
        }
        setHtml(html);
    } else {
        qWarning() << "not allowed." << path;
    }
}

QString TextPreview::fileName() const {
    return QFileInfo(m_filePath).fileName();
}

QString TextPreview::filePath() const {
    return m_filePath;
}

QString TextPreview::title() const
{
    return m_title;
}

void TextPreview::setHtml(const QString &html) {
    int renderMode = Settings::instance()->modeRender;
    if (renderMode == 0) {
#ifdef USE_WEB_ENGINE_VIEW
    m_webEngineView->setHtml(html, QUrl("file://" + QFileInfo(m_filePath).absolutePath() + '/'));
#endif
    } else if (renderMode == 1) {
        m_textBrowser->setHtml(html);
    }
}

void TextPreview::reload() {
    qDebug() << "reload" << m_filePath;
    int renderMode = Settings::instance()->modeRender;
    if (renderMode == 2) {
        m_editor->reload();
    } else if (renderMode == 1) {
        loadFile(m_filePath);
    }
}

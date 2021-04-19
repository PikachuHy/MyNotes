//
// Created by pikachu on 2021/4/19.
//

#include "TextPreview.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtMarkdownParser/QtMarkdownParser>
#include <QStandardPaths>
TextPreview::TextPreview(QWidget *parent) : QWebEngineView(parent) {

}

void TextPreview::loadFile(const QString &path) {
    m_filePath = path;
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
    setHtml(allHtml, QUrl("file://" + QFileInfo(path).absolutePath() + '/'));
}

QString TextPreview::fileName() const {
    return QFileInfo(m_filePath).fileName();
}

QString TextPreview::filePath() const {
    return m_filePath;
}

#include "export/HtmlExporter.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include "QtMarkdownParser"

HtmlExporter::HtmlExporter(const QString& workshopPath, QObject *parent)
    : QObject(parent), m_workshopPath(workshopPath)
{
}

QString HtmlExporter::noteRealPath(const Note& note) const
{
    return m_workshopPath + note.strId() + "/index.md";
}

QString HtmlExporter::generateHtml(const Note& note)
{
    return generateHtml(noteRealPath(note), note.title());
}

QString HtmlExporter::generateHtml(const QString& mdPath, const QString& title)
{
    QFile mdFile(mdPath);
    bool ok = mdFile.open(QIODevice::ReadOnly);
    if (!ok) {
        qWarning() << "open fail:" << mdPath;
        return {};
    }
    Document doc(mdFile.readAll());
    mdFile.close();
    auto html = doc.toHtml();
    QFile cssFile(":css/css518.css");
    cssFile.open(QIODevice::ReadOnly);
    QString css = cssFile.readAll();
    cssFile.close();
    return R"(<!DOCTYPE html><html><head>
<meta charset="utf-8">
<meta name='viewport' content='width=device-width initial-scale=1'>
<title>)"
        + title
        + R"(</title>
<link href='https://fonts.loli.net/css?family=Open+Sans:400italic,700italic,700,400&subset=latin,latin-ext' rel='stylesheet' type='text/css' />

<style type='text/css'>)"
        + css
        + R"("</style>
</head>
<body class='typora-export'>
<div id='write'  class=''>)"
        + html
        + R"(</div></body></html>)";
}

void HtmlExporter::exportToHtml(const Note& note, const QString& dirPath)
{
    QString notePath = noteRealPath(note);
    QDir noteDir(notePath);
    noteDir.cdUp();
    QFileInfoList fileInfoList = noteDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& fileInfo : fileInfoList) {
        if (fileInfo.fileName() == "index.md") continue;
        QString targetFile = QDir(dirPath).filePath(fileInfo.fileName());
        QFile::copy(fileInfo.filePath(), targetFile);
    }
    QString htmlPath = QDir(dirPath).filePath("index.html");
    QFile htmlFile(htmlPath);
    htmlFile.open(QIODevice::WriteOnly);
    htmlFile.write(generateHtml(notePath, note.title()).toUtf8());
    htmlFile.close();
    QDesktopServices::openUrl(QUrl(QString("file://%1").arg(dirPath)));
}

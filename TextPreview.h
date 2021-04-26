//
// Created by pikachu on 2021/4/19.
//

#ifndef MYNOTES_TEXTPREVIEW_H
#define MYNOTES_TEXTPREVIEW_H
#ifdef USE_WEB_ENGINE_VIEW
#include <QWebEngineView>
#endif
#include <QTextBrowser>
#include <QWidget>
class Editor;
class TextPreview : public QWidget{
Q_OBJECT
public:
    explicit TextPreview(QWidget* parent = nullptr);
    void loadFile(const QString& path);
    void reload();
    QString fileName() const;
    QString filePath() const;
private:
    QString m_filePath;
    void setHtml(const QString& html);
#ifdef USE_WEB_ENGINE_VIEW
    QWebEngineView *m_webEngineView;
#endif
    QTextBrowser *m_textBrowser;
    Editor *m_editor;
};


#endif //MYNOTES_TEXTPREVIEW_H

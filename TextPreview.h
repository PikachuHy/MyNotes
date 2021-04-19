//
// Created by pikachu on 2021/4/19.
//

#ifndef MYNOTES_TEXTPREVIEW_H
#define MYNOTES_TEXTPREVIEW_H
#ifdef USE_WEB_ENGINE_VIEW
#include <QWebEngineView>
#else
#include <QTextBrowser>
#endif
#include <QWidget>

class TextPreview : public QWidget{
Q_OBJECT
public:
    explicit TextPreview(QWidget* parent = nullptr);
    void loadFile(const QString& path);
    QString fileName() const;
    QString filePath() const;
private:
    QString m_filePath;
#ifdef USE_WEB_ENGINE_VIEW
    QWebEngineView *m_webEngineView;
#else
    QTextBrowser *m_textBrowser;
#endif
};


#endif //MYNOTES_TEXTPREVIEW_H

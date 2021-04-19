//
// Created by pikachu on 2021/4/19.
//

#ifndef MYNOTES_TEXTPREVIEW_H
#define MYNOTES_TEXTPREVIEW_H
#include <QWebEngineView>
#include <QWidget>

class TextPreview : public QWebEngineView{
Q_OBJECT
public:
    explicit TextPreview(QWidget* parent = nullptr);
    void loadFile(const QString& path);
    QString fileName() const;
    QString filePath() const;
private:
    QString m_filePath;
};


#endif //MYNOTES_TEXTPREVIEW_H

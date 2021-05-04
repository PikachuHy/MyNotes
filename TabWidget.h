//
// Created by pikachu on 2021/4/19.
//

#ifndef MYNOTES_TABWIDGET_H
#define MYNOTES_TABWIDGET_H

#include <QTabBar>
#include <QTabWidget>
class TextPreview;
class TabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit TabWidget(QWidget* parent = nullptr);
    void add(TextPreview* tab, const QString& title);

    TextPreview *currentTab() const;
    TextPreview *tabAt(int index) const;
    QVector<TextPreview*> tabs() const;
    int numTabs() const { return tabs().length(); }
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
};


#endif //MYNOTES_TABWIDGET_H

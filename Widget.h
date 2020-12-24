#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QAction>

class TreeModel;

class Widget : public QWidget {
Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    ~Widget();

private:
    QLayout *initTitleLayout();

    void initSlots();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:

    void on_treeView_activated(const QModelIndex &index);

    void on_treeView_customContextMenuRequested(const QPoint &pos);

private:
    inline QString attachmentPath() {
        return m_notesPath + "/attachment";
    }

private:
    QTreeView *m_treeView;
    QTextEdit *m_textEdit;
    QTextBrowser *m_textPreview;
    QLabel *m_titleLabel;
    QLineEdit *m_titleLineEdit;
    QPushButton *m_toggleEditAndPreview;
    QPushButton *m_showEditButton;
    QPushButton *m_showPreviewButton;
    TreeModel *m_treeModel;
    QString m_notesPath;
    QString m_curNotePath;
};

#endif // WIDGET_H

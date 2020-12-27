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
#include <QSqlQuery>

class TreeItem;
class TreeModel;
class DbManager;
class QSqlRelationalTableModel;
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

    void on_treeView_pressed(const QModelIndex &index);

    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_action_newNote();

    void on_action_newFolder();

    void on_action_trashNote();

    void on_action_trashFolder();
private:
    inline QString attachmentPath() {
        return m_notesPath + "/attachment/";
    }
    inline QString workshopPath() {
        return m_notesPath + "/workshop/";
    }
    inline QString tmpHtmlPath() {
        return m_notesPath + "/tmp/note.html";
    }
    inline QString trashPath() {
        return m_notesPath + "/trash/";
    }
    inline QString imageMdText(const QString & url) {
        return "![image]("+url+")";
    }
    inline TreeItem* currentTreeItem();
    inline void showErrorDialog(const QString & msg);
    void loadMdText();
    void saveMdText();
    void updatePreview();
    void updateIndex(QString text, int id);


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
    QString m_curCheckedPath;
    TreeItem* m_curItem;
    QModelIndex m_curIndex;
    DbManager* m_dbManager;
    int m_curNoteId;
};

#endif // WIDGET_H

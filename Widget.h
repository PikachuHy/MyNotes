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
//#include "cppjieba/Jieba.hpp"
#include "DbModel.h"
#include <QSettings>
#include <QFile>
#include <QFileSystemWatcher>
class SyncService;
class NoteFileService;
class TreeItem;
class TreeModel;
class DbManager;
class QSqlRelationalTableModel;
class SearchController;
class Settings;
class FileSystemWatcher;
class TrayIconManager;
class WebEngineView;
#ifdef ENABLE_TROJAN
class TrojanThread;
#endif
class TabWidget;
#include <QTimer>
#include "PiWidget.h"
class Indexer;
class HtmlExporter;
class Widget : public PiWidget {
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget();

private:

    void initSlots();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showNextTab();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:

    void on_treeView_pressed(const QModelIndex &index);

    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_action_newNote();

    void on_action_newFolder();

    void on_action_trashNote();

    void on_action_openInTypora();

    void on_action_trashFolder();

    void on_action_exportNoteToHTML();

    void on_fileSystemWatcher_fileChanged(const QString &path);

private:
    inline QString attachmentPath() {
        return m_notesPath + "/attachment/";
    }
    inline QString workshopPath() {
        return m_notesPath + "/workshop/";
    }
    inline QString tmpPath() {
        return m_notesPath + "/tmp/";
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
    void loadNote(const Note& note);
    void loadNote(const QString& path);
    void loadMdText();
    void loadMdText(const QString& path);
    void saveMdText();
    void updatePreview();
    void updatePreview(const QString& path);
    void updateIndex(QString text, int id);
    void initJieba();
    void updateStatistics();
    void openNoteInTypora(const Note& note);
    void openInTypora(const QString& path);
//    Jieba* jieba();
    QString currentNotePath();
    void initFileSystemWatcher();
    void initShortcut();
private:
    // 左边到文档树
    QTreeView *m_treeView;
    QTextEdit *m_textEdit;
    // markdown转html后预览，用GitHub到样式
    WebEngineView *m_textPreview;
    TabWidget *m_tabWidget;
    QLabel *m_wordCountLabel;
    TreeModel *m_treeModel;
    // 笔记所在路径
    QString m_notesPath;
    // SQLite数据库操作
    DbManager* m_dbManager;
    // 双击Shift出现搜索框用
    time_t m_lastPressShiftTime;
    time_t m_maxShiftInterval;
    // 分词
//    Jieba* m_jieba;
    Note m_curNote;
    bool m_showOpenInTyporaTip;
    // 持久化的配置
    Settings *m_settings;

    void loadLastOpenedNote();
    FileSystemWatcher* m_fileSystemWatcher;
    TrayIconManager* m_trayIconManager;
    QString m_curNotePath;
    QTimer* m_timer;
    Indexer* m_indexer;
    SearchController* m_searchController;
    HtmlExporter* m_htmlExporter;
    SyncService* m_syncService;
    NoteFileService* m_noteFileService;
#ifdef ENABLE_TROJAN
    TrojanThread* m_trojanThread;
#endif
};

#endif // WIDGET_H

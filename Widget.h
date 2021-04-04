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
#include <QWebEngineView>
#include <QSettings>
#include <QFile>
#include <QFileSystemWatcher>
#include "ElasticSearchRestApi.h"
#include <QSystemTrayIcon>
class TreeItem;
class TreeModel;
class DbManager;
class QSqlRelationalTableModel;
class SearchDialog;
//using cppjieba::Jieba;
class QListView;
class ListModel;
class ElasticSearchRestApi;
class Settings;
class FileSystemWatcher;
#include "PiWidget.h"
class Widget : public PiWidget {
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget();

private:

    void initSlots();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:

    void on_treeView_pressed(const QModelIndex &index);

    void on_listView_pressed(const QModelIndex &index);

    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_action_newNote();

    void on_action_newFolder();

    void on_action_trashNote();

    void on_action_openInTypora();

    void on_action_trashFolder();

    void on_action_exportNoteToHTML();

    void on_searchDialog_searchTextChanged(const QString& text);

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
    void initSearchDialog();
    void initJieba();
    void updateStatistics();
    void openNoteInTypora(const Note& note);
    void openInTypora(const QString& path);
    QListView* searchResultView();
//    Jieba* jieba();
    QString currentNotePath();
    QString noteRealPath(const Note& note);
    QString generateHTML(const Note& note);
    QString generateHTML(const QString& path, const QString& title);
    void generateHTML(const Note& note, const QString& path);
    void addNoteTo();
    void uploadNoteAttachment(const Note& note);
    void initSystemTrayIcon();
    void initFileSystemWatcher();
    void syncAll();
    void syncAllWatching();
    void syncWatchingFile(const QString& path);
    void syncWatchingFolder(const QString& path);
    void showSyncResult(const QString& msg);
    void setAutoStart();
    void uploadFile(const QString& noteStrId, const QString& path);
    void uploadNote(const ServerNoteInfo& info);
    void syncWorkshopFile(const Note& note);
private:
    // 左边到文档树
    QTreeView *m_treeView;
    QTextEdit *m_textEdit;
    // markdown转html后预览，用GitHub到样式
    QWebEngineView *m_textPreview;
    QLabel *m_wordCountLabel;
    TreeModel *m_treeModel;
    // 笔记所在路径
    QString m_notesPath;
    // SQLite数据库操作
    DbManager* m_dbManager;
    // 双击Shift出现搜索框用
    time_t m_lastPressShiftTime;
    time_t m_maxShiftInterval;
    SearchDialog* m_searchDialog;
    // 分词
//    Jieba* m_jieba;
    QListView* m_listView;
    ListModel* m_listModel;
    Note m_curNote;
    bool m_showOpenInTyporaTip;
    // 持久化的配置
    Settings *m_settings;

    void loadLastOpenedNote();
    ElasticSearchRestApi* m_esApi;
    FileSystemWatcher* m_fileSystemWatcher;
    QSystemTrayIcon* m_systemTrayIcon;
};

#endif // WIDGET_H

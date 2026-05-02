#ifndef MYNOTES_NOTEEDITORWIDGET_H
#define MYNOTES_NOTEEDITORWIDGET_H

#include <QWidget>
#include "database/DbModel.h"

class QTextEdit;
class TabWidget;
class TextPreview;
class Indexer;
class NoteFileService;
class FileSystemWatcher;
class Settings;
class DbManager;
class MarkdownHighlighter;

class NoteEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit NoteEditorWidget(QWidget *parent = nullptr);

    void setDependencies(Indexer* indexer, NoteFileService* noteFileService,
                         FileSystemWatcher* fileSystemWatcher, Settings* settings, DbManager* dbManager,
                         const QString& workshopPath);

    void loadNote(const Note& note);
    void loadNote(const QString& filePath);
    void save();
    void refreshPreview();
    QString currentFilePath() const;
    void showNextTab();
    void openInTypora(const QString& path);
    void reloadTabAt(const QString& path);
    void loadLastOpenedNote();

signals:
    void noteModified(const QString& path);
    void linkClicked(const QString& link);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void loadMdText();
    void loadMdText(const QString& path);
    void updatePreview();
    void updatePreview(const QString& path);
    void updateIndex(const QString& text, int noteId);
    void updateStatistics();
    QString currentNoteStrId() const;

    QTextEdit *m_textEdit;
    TabWidget *m_tabWidget;
    Note m_curNote;
    QString m_curNotePath;
    Indexer* m_indexer;
    NoteFileService* m_noteFileService;
    FileSystemWatcher* m_fileSystemWatcher;
    Settings* m_settings;
    DbManager* m_dbManager;
    QString m_workshopPath;
    bool m_showOpenInTyporaTip;
    MarkdownHighlighter *m_highlighter = nullptr;
    QTimer *m_debounceTimer = nullptr;
};

#endif

#include "NoteEditorWidget.h"
#include "notes/NoteFileService.h"
#include "watcher/FileSystemWatcher.h"
#include "settings/Settings.h"
#include "indexer/Indexer.h"
#include "database/DbManager.h"
#include "utils/Utils.h"
#include "utils/Toast.h"
#include "TabWidget.h"
#include "TextPreview.h"
#include "MarkdownHighlighter.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QKeyEvent>
#include <QUrl>
#include <QUrlQuery>
#include <QtConcurrent>
#include <QVector>

NoteEditorWidget::NoteEditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_showOpenInTyporaTip(true)
{
    m_textEdit = new QTextEdit();
    m_textEdit->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");
    m_tabWidget = new TabWidget();
    connect(m_tabWidget, &TabWidget::tabCloseRequested, [this](int index){
        m_tabWidget->removeTab(index);
    });
    auto layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_textEdit);
    layout->addWidget(m_tabWidget);
    setLayout(layout);

    // Syntax highlighting
    m_highlighter = new MarkdownHighlighter(m_textEdit->document(), this);

    // Real-time preview with 300ms debounce
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(300);
    connect(m_debounceTimer, &QTimer::timeout, this, &NoteEditorWidget::refreshPreview);
    connect(m_textEdit, &QTextEdit::textChanged, this, [this]() {
        m_debounceTimer->start();
    });

    m_textEdit->installEventFilter(this);
}

void NoteEditorWidget::setDependencies(Indexer* indexer, NoteFileService* noteFileService,
                         FileSystemWatcher* fileSystemWatcher, Settings* settings, DbManager* dbManager,
                         const QString& workshopPath) {
    m_indexer = indexer;
    m_noteFileService = noteFileService;
    m_fileSystemWatcher = fileSystemWatcher;
    m_settings = settings;
    m_dbManager = dbManager;
    m_workshopPath = workshopPath;
}

void NoteEditorWidget::loadLastOpenedNote() {
    QTimer::singleShot(50, [this](){
        QString lastOpenNotePath = m_settings->lastOpenNotePath;
        if (lastOpenNotePath.isEmpty()) {
            qDebug() << "no last note";
            return ;
        }
        if (!QFile(lastOpenNotePath).exists()) {
            qInfo() << "last open note not exist." << lastOpenNotePath;
            return;
        }
        loadNote(lastOpenNotePath);
    });
}

void NoteEditorWidget::loadNote(const Note &note) {
    qDebug() << "load" << note.strId() << note.title();
    m_curNote = note;
    const QString &path = m_noteFileService->noteRealPath(note);
    loadNote(path);
    if (m_showOpenInTyporaTip) {
        Toast::showTip("Press E Open in Typora", this);
        m_showOpenInTyporaTip = false;
    }
}

void NoteEditorWidget::loadNote(const QString &path) {
    if (!QFile(path).exists()) {
        qWarning() << "note not exist." << path;
    }
    m_fileSystemWatcher->addPath(path);
    m_curNotePath = path;
    qDebug() << "load" << path;
    loadMdText(path);
    updatePreview(path);
    m_settings->lastOpenNotePath = path;
}

void NoteEditorWidget::loadMdText() {
    loadMdText(currentFilePath());
}

void NoteEditorWidget::loadMdText(const QString &notePath) {
    QFile file(notePath);
    if(!file.exists()) {
        qDebug() << notePath << "is not exist.";
        QMessageBox::critical(this, tr("File"), tr("File not exist."));
        return;
    }
    bool ret = file.open(QIODevice::ReadOnly);
    if (!ret) {
        qDebug() << notePath << "open fail.";
        QMessageBox::critical(this, tr("File"), tr("File open fail."));
        return;
    }
    QString mdText = file.readAll();
    m_textEdit->setText(mdText);
    file.close();
    updateStatistics();
}

void NoteEditorWidget::save() {
    auto notePath = currentFilePath();
    if (notePath.isEmpty()) {
        return;
    }
    QFile file(notePath);
    file.open(QIODevice::WriteOnly);
    const QString &mdText = m_textEdit->toPlainText();
    file.write(mdText.toUtf8());
    file.close();
    updateIndex(mdText, m_curNote.id());
    updateStatistics();
    emit noteModified(notePath);
}

void NoteEditorWidget::refreshPreview() {
    updatePreview(currentFilePath());
}

void NoteEditorWidget::updatePreview() {
    updatePreview(currentFilePath());
}

void NoteEditorWidget::updatePreview(const QString& path) {
    auto tabs = m_tabWidget->tabs();
    for(auto tab: tabs) {
        if (tab->filePath() == path) {
            m_tabWidget->setCurrentWidget(tab);
            return;
        }
    }
    TextPreview* textPreview = new TextPreview();
    connect(textPreview, &TextPreview::linkClicked, [this](QString link){
        QUrl url(link);
        auto scheme = url.scheme();
        if (scheme != "note") {
            return;
        }
        auto q = url.query();
        qDebug() << "query" << q;
        QUrlQuery urlQuery(q);
        auto noteStrId = urlQuery.queryItemValue("note");
        qDebug() << "note id" << noteStrId;
        auto pathId = urlQuery.queryItemValue("path");
        auto note = this->m_dbManager->getNote(noteStrId);
        if (note.id() == -1) {
            QMessageBox::warning(this, tr("Open Note"), tr("Note not exist.\nNote Str ID: %1").arg(noteStrId));
        } else {
            loadNote(note);
        }
        emit linkClicked(link);
    });
    textPreview->loadFile(path);
    QString title = QFileInfo(path).fileName();
    if (path.startsWith(m_workshopPath)) {
        auto strId = m_noteFileService->noteStrIdFromWorkshopPath(path);
        auto note = m_dbManager->getNote(strId);
        title = note.title();
    }
    m_tabWidget->add(textPreview, title);
}

QString NoteEditorWidget::currentFilePath() const {
    return m_curNotePath;
}

void NoteEditorWidget::updateIndex(const QString& text, int id) {
    QtConcurrent::run([this, text, id]() {
        m_indexer->updateIndex(id, text);
        m_indexer->saveIndex();
        qDebug() << "index updated for note" << id;
    });
}

void NoteEditorWidget::updateStatistics() {
}

void NoteEditorWidget::openInTypora(const QString& notePath) {
    qDebug() << "open in typora:" << notePath;
#ifdef Q_OS_WIN
    QStringList pathList;
    QString typoraPath = m_settings->typoraPath;
    pathList << typoraPath;
    pathList << "C:\\Program Files\\Typora\\Typora.exe";
    pathList << "C:\\Program Files (x86)\\Typora\\Typora.exe";
    pathList << "D:\\typora\\Typora\\Typora.exe";
    QString exePath;
    bool exeFind = false;
    for(auto path: pathList) {
        if (QFile(path).exists()) {
            exeFind = true;
            exePath = path;
        }
    }
    if (!exeFind) {
        QMessageBox::critical(this, tr("ERROR"), tr("Please install Typora first."));
    } else {
        QStringList cmd;
        cmd << notePath;
        QProcess::startDetached(exePath,cmd);
    }
#else
    QStringList cmd;
    cmd << "-a" << "typora" << notePath;
    QProcess::startDetached("open",cmd);
#endif
}

void NoteEditorWidget::showNextTab() {
    qDebug() << "next tab";
    int newTabIndex = (m_tabWidget->currentIndex() + 1) % m_tabWidget->count();
    m_tabWidget->setCurrentWidget(m_tabWidget->tabAt(newTabIndex));
}

void NoteEditorWidget::reloadTabAt(const QString& path) {
    for(auto tab: m_tabWidget->tabs()) {
        if (tab->filePath() == path) {
            tab->reload();
        }
    }
}

bool NoteEditorWidget::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::KeyPress) {
        auto *event = static_cast<QKeyEvent *>(e);
        if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier)) {
            save();
            return true;
        }
        if (event->key() == Qt::Key_E) {
            openInTypora(currentFilePath());
        }
    }
    return QObject::eventFilter(watched, e);
}

QString NoteEditorWidget::currentNoteStrId() const {
    QString path = m_curNotePath;
    if (path.startsWith(m_workshopPath)) {
        QStringList segs = path.split('/');
        return segs[segs.size() - 2];
    } else {
        return Utils::md5(path);
    }
}

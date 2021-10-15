
#include "TreeModel.h"
#include "TreeItem.h"
#include "Constant.h"
#include "DbManager.h"
#include "FileSystemWatcher.h"
#include "Utils.h"
#include <QStringList>
#include <QDir>
#include <QDebug>

TreeModel::TreeModel(const QString &path, DbManager* dbManager, QObject *parent)
        : QAbstractItemModel(parent), m_dataPath(path), m_dbManager(dbManager)
        , m_settings(Settings::instance())
        , m_watchingItem(nullptr) {
    m_dataPath = path;
    ensurePathExist(path);
    ensurePathExist(path + "/" + Constant::workshop);
    ensurePathExist(path + "/" + Constant::attachment);
    ensurePathExist(path + "/" + Constant::trash);
    ensurePathExist(path + "/" + Constant::tmp);
    rootItem = new TreeItem({"File"});
    setupModelData(rootItem);
}

TreeModel::~TreeModel() {
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::DecorationRole && role != IconPathRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    return item->data(index.column(), role);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section, role);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex TreeModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (!idx.isValid()) {
        return QModelIndex();
    }
    auto parentIndex = idx.parent();
    if (!parentIndex.isValid()) {
        return createIndex(row, column, rootItem->child(row));
    }
    if (!hasIndex(row, column, idx))
        return QModelIndex();
    TreeItem *parentItem = static_cast<TreeItem *>(parentIndex.internalPointer());
    return createIndex(row, column, parentItem->child(row));
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    return parentItem->childCount();
}

void buildFileTree(QString path, TreeItem *parent) {
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << dir << "not exist.";
        return;
    }
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < info_list.count(); i++) {
        auto info = info_list[i];
        QList<QVariant> data;
        data << info.fileName();
        auto child = new TreeItem(data, parent);
        child->setPath(info.absoluteFilePath());
        if (info.isDir()) {
            buildFileTree(info.absoluteFilePath(), child);
        }
        parent->appendChild(child);
    }
}

void TreeModel::setupModelData(TreeItem *parent) {
    auto workshopItem = new WorkshopItem(workshopPath(), parent);
    parent->appendChild(workshopItem);
    m_watchingItem = new WatchingItem(parent);
    QStringList watchingDirs = Settings::instance()->watchingFolders;
    for(const QString& watchingDir: watchingDirs) {
        FileSystemWatcher::instance()->addPath(watchingDir);
        auto dirName = QDir(watchingDir).dirName();
        auto watchingFolder = new WatchingFolderItem(watchingDir, dirName, m_watchingItem);
        m_path2item[watchingDir] = watchingFolder;
        m_watchingItem->appendChild(watchingFolder);
        buildWatchingTree(watchingDir, watchingFolder);
    }
    parent->appendChild(m_watchingItem);
    parent->appendChild(new AttachmentItem(attachmentPath(), parent));
    parent->appendChild(new TrashItem(trashPath(), parent));
//    buildFileTree(workshopPath(), workshopItem);
    buildFileTreeFromDb(0, workshopItem);
}

void TreeModel::ensurePathExist(QString path) {
    if (!QFileInfo(path).exists()) {
        auto ret = QDir().mkpath(path);
        if (!ret) {
            qDebug() << "create dir fail: " << path;
            exit(0);
        }
    }
}

void TreeModel::buildWatchingTree(QString path, TreeItem *parent)
{
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << dir << "not exist.";
        return;
    }
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < info_list.count(); i++) {
        auto info = info_list[i];
        Utils::syncSuffix();
        const QString &filePath = info.absoluteFilePath();
        FileSystemWatcher::instance()->addPath(filePath);
        QString data = info.fileName();
        TreeItem* child;
        if (info.isDir()) {
            child = new WatchingFolderItem(filePath, data, parent);
            child->setPath(filePath);
            buildWatchingTree(filePath, child);
        } else {
            bool ok = false;
            for(const auto& suffix: Utils::syncSuffix()) {
                if (filePath.endsWith(suffix)) {
                    child = new WatchingFileItem(filePath, info.fileName(), parent);
                    child->setPath(filePath);
                    ok = true;
                    break;
                }
            }
            if (!ok) continue;
        }
        m_path2item[filePath] = child;
        parent->appendChild(child);
    }

}

QString TreeModel::workshopPath() {
    return m_dataPath + "/" + Constant::workshop;
}

QString TreeModel::trashPath() {
    return m_dataPath + "/" + Constant::trash;
}

QString TreeModel::attachmentPath() {
    return m_dataPath + "/" + Constant::attachment;
}

QModelIndex TreeModel::addNewNode(const QModelIndex& parent, TreeItem* child) {
    if (!parent.isValid()) {
        qDebug() << "index is invalid";
        return QModelIndex();
    }
    auto parentItem = static_cast<TreeItem *>(parent.internalPointer());
    int insertRowNum = parentItem->childCount();
    beginInsertRows(parent, insertRowNum, insertRowNum);
    parentItem->appendChild(child);
    endInsertRows();
    return index(insertRowNum, 0, parent);
}

QModelIndex TreeModel::addNewFolder(const QModelIndex &parent, TreeItem *child) {
    if (!parent.isValid()) {
        qDebug() << "index is invalid";
        return QModelIndex();
    }
    auto parentItem = static_cast<TreeItem *>(parent.internalPointer());

    int insertRowNum = parentItem->insertFolder(child);
    beginInsertRows(parent, insertRowNum, insertRowNum);
    endInsertRows();
    return index(insertRowNum, 0, parent);
}

void TreeModel::removeNode(const QModelIndex &index) {
    auto item = static_cast<TreeItem *>(index.internalPointer());
    int rowToBeRemoved = item->indexInParent();
    beginRemoveRows(index.parent(), rowToBeRemoved, rowToBeRemoved);
    item->parentItem()->removeChild(item);
    endRemoveRows();
}

void TreeModel::addWatchingDir(const QModelIndex& parent, const QString &watchingDir)
{
    int insertRowNum = m_watchingItem->childCount();
    qDebug() << "watching row: " << insertRowNum;
     beginInsertRows(parent, insertRowNum, insertRowNum);
     auto dirName = QDir(watchingDir).dirName();
     auto watchingFolder = new WatchingFolderItem(watchingDir, dirName, m_watchingItem);
     m_watchingItem->appendChild(watchingFolder);
     m_path2item[watchingDir] = watchingFolder;
     buildWatchingTree(watchingDir, watchingFolder);
     endInsertRows();
}
void TreeModel::buildFileTreeFromDb(int parentPathId, TreeItem *parentItem) {
    auto pathList = m_dbManager->getPathList(parentPathId);
    for(const auto& path: pathList) {
        if(path.trashed()) continue;
        QString realPath = parentItem->path() + "/" + path.name();
        auto item = new FolderItem(path, parentItem);
        item->setPath(realPath);
        parentItem->appendChild(item);
        buildFileTreeFromDb(path.id(), item);
    }
    auto noteList = m_dbManager->getNoteList(parentPathId);
    for(const auto& note: noteList) {
        if (note.trashed()) continue;
        QString realPath = parentItem->path() + "/" + note.title();
        auto item = new NoteItem(note, parentItem);
        item->setPath(realPath);
        parentItem->appendChild(item);
    }
}

void TreeModel::updateWatchingDir(const QString &oldPath, const QString &newPath) {
    if (m_path2item.contains(oldPath)) {
        auto item = m_path2item[oldPath];
        if (item) {
            qDebug() << "update watching dir." << "from" << oldPath << "to" << newPath;
            item->setPath(newPath);
            item->setDisplayName(QDir(newPath).dirName());
            m_path2item.remove(oldPath);
            m_path2item[newPath] = item;
            auto row = item->indexInParent();
            beginRemoveRows(createIndex(row, 0, item), row, row);
            endRemoveRows();
        } else {
            qWarning() << "update watching dir error." << "item in nullptr";
        }
    } else {
        qWarning() << "update watching dir error." << oldPath << "not in map.";
    }
}

void TreeModel::addWatchingNode(const QString &path) {
    if (m_path2item.contains(path)) {
        qWarning() << "path already in watching tree." << path;
    } else {
        const QFileInfo fileInfo(path);
        auto parentPath = fileInfo.absoluteDir().absolutePath();
        if (m_path2item.contains(parentPath)) {
            auto parentItem = m_path2item[parentPath];
            auto row = parentItem->childCount();
            auto parentIndex = createIndex(parentItem->indexInParent(), 0, parentItem);
            TreeItem* child;
            const QString &filename = fileInfo.fileName();
            if (fileInfo.isDir()) {
                child = new WatchingFolderItem(path, filename, parentItem);
            } else {
                child = new WatchingFileItem(path, filename, parentItem);
            }
            beginInsertRows(parentIndex, row, row);
            parentItem->appendChild(child);
            endInsertRows();
            m_path2item[path] = child;
            FileSystemWatcher::instance()->addPath(path);
        } else {
            qWarning() << "parent path not in map." << parentPath;
        }
    }
}

void TreeModel::removeWatchingNote(const QString &path) {
    if (m_path2item.contains(path)) {
        auto item = m_path2item[path];
        auto row = item->indexInParent();
        auto index = createIndex(row, 0, item);
        beginRemoveRows(index, row, row);
        auto parentItem = item->parentItem();
        if (parentItem) {
            parentItem->removeChild(item);
        } else {
            qWarning() << "parent item is nullptr." << path;
        }
        endRemoveRows();
    } else {
        qWarning() << "path not in watching tree." << path;
    }
}

QHash<int, QByteArray> TreeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[IconPathRole] = "iconPath";
    return roles;
}

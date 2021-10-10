
#include "TreeItem.h"
#include "TreeModel.h"
#include "Constant.h"
#include <QFileInfo>
#include <QFileIconProvider>
#include <QIcon>

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
        : m_itemData(data), m_parentItem(parent) {}

TreeItem::~TreeItem() {
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item) {
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int TreeItem::childCount() const {
    return m_childItems.count();
}

int TreeItem::columnCount() const {
    return m_itemData.count();
}

QVariant TreeItem::data(int column, int role) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

TreeItem *TreeItem::parentItem() {
    return m_parentItem;
}

int TreeItem::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem *>(this));

    return 0;
}

void TreeItem::setPath(QString path) {
    m_path = path;
}

QString TreeItem::path() {
    return m_path;
}

bool TreeItem::isTrashItem() {
    return false;
}

bool TreeItem::isAttachmentItem() {
    return false;
}

bool TreeItem::isWorkshopItem() {
    return false;
}

bool TreeItem::isFile() {
    return !m_isFolder;
}

int TreeItem::insertFolder(TreeItem *child) {
    int i = 0;
    for(auto item: m_childItems) {
        if (item->isFolderItem() || QFileInfo(item->path()).isDir()) {
            i++;
        } else {
            break;
        }
    }
    m_childItems.insert(i, child);
    return i;
}

int TreeItem::removeChild(TreeItem* child) {
    int rowToBeRemoved = m_childItems.indexOf(child);
    m_childItems.removeOne(child);
    return rowToBeRemoved;
}

int TreeItem::indexInParent() {
    if (!m_parentItem) return -1;
    return m_parentItem->m_childItems.indexOf(this);
}

int TreeItem::pathId() {
    return 0;
}

void TreeItem::setDisplayName(const QString& name) {
    m_itemData[0] = name;
}

QVariant TrashItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::trashImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::trashImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Trashcan);
#endif
    }
    return TreeItem::data(column, role);
}

bool TrashItem::isTrashItem() {
    return true;
}

TrashItem::TrashItem(const QString & path, TreeItem *parentItem) : TreeItem({"Trash"}, parentItem) {
    setPath(path);
    m_isFolder = true;
}

bool TrashItem::isFile() {
    return false;
}

QVariant AttachmentItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::attachmentImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::attachmentImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Drive);
#endif
    }
    return TreeItem::data(column, role);
}

bool AttachmentItem::isAttachmentItem() {
    return true;
}

AttachmentItem::AttachmentItem(const QString & path, TreeItem *parentItem) : TreeItem({"Attachment"}, parentItem) {
    setPath(path);
}

bool AttachmentItem::isFile() {
    return false;
}

QVariant WorkshopItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole) {
        return Constant::workshopImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::workshopImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Computer);
#endif
    }
    return TreeItem::data(column, role);
}

bool WorkshopItem::isWorkshopItem() {
    return true;
}

WorkshopItem::WorkshopItem(const QString & path, TreeItem *parentItem) : TreeItem({"Workshop"}, parentItem) {
    setPath(path);
    m_isFolder = true;
}

bool WorkshopItem::isFile() {
    return false;
}

bool NoteItem::isFile() {
    return true;
}

NoteItem::NoteItem(Note note, TreeItem *parentItem): TreeItem({note.title()}, parentItem), m_note(note) {

}

int NoteItem::pathId() {
    return m_note.pathId();
}

QVariant NoteItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::noteImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::noteImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::File);
#endif
    }
    return TreeItem::data(column, role);
}

QString NoteItem::path() {
    return m_note.strId();
}

bool FolderItem::isFile() {
    return false;
}

FolderItem::FolderItem(Path path, TreeItem *parentItem): TreeItem({path.name()}, parentItem), m_path(path) {
    m_isFolder = true;
}

int FolderItem::pathId() {
    return m_path.id();
}

QVariant FolderItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::folderImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::folderImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Folder);
#endif
    }
    return TreeItem::data(column, role);
}


WatchingItem::WatchingItem(TreeItem *parentItem): TreeItem({"Watching"}, parentItem) {
    m_isFolder = true;
}

QVariant WatchingItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::watchingImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::watchingImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Folder);
#endif
    }
    return TreeItem::data(column, role);
}

WatchingFolderItem::WatchingFolderItem(const QString& path, const QString& displayName, TreeItem *parentItem): TreeItem({displayName}, parentItem){
    setPath(path);
    m_isFolder = true;
}

QVariant WatchingFolderItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::folderImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::folderImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Folder);
#endif
    }
    return TreeItem::data(column, role);
}

WatchingFileItem::WatchingFileItem(const QString &path, const QString& displayName, TreeItem *parentItem): TreeItem({displayName}, parentItem) {
    setPath(path);
}

QVariant WatchingFileItem::data(int column, int role) const {
    if (role == TreeModel::IconPathRole && column == 0) {
        return Constant::noteImagePath;
    }
    if (role == Qt::DecorationRole && column == 0) {
#ifdef Q_OS_WIN
        return QIcon(QPixmap(Constant::noteImagePath));
#else
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::File);
#endif
    }
    return TreeItem::data(column, role);
}

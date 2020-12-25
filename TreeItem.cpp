
#include "TreeItem.h"
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
    if (role == Qt::DecorationRole && column == 0) {
        if (m_path.isEmpty()) return QVariant();

        QFileInfo fileInfo(m_path);
        if (fileInfo.exists()) {
            QFileIconProvider iconProvider;
            return iconProvider.icon(fileInfo);
        } else {
            qDebug() << "file" << m_path << "not exist";
            return QVariant();
        }
    }
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
    return QFileInfo(m_path).isFile();
}

int TreeItem::insertFolder(TreeItem *child) {
    int i = 0;
    for(auto item: m_childItems) {
        if (QFileInfo(item->path()).isDir()) {
            i++;
        }
        break;
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

QVariant TrashItem::data(int column, int role) const {
    if (role == Qt::DecorationRole && column == 0) {
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Trashcan);
    }
    return TreeItem::data(column, role);
}

bool TrashItem::isTrashItem() {
    return true;
}

TrashItem::TrashItem(const QString & path, TreeItem *parentItem) : TreeItem({"Trash"}, parentItem) {
    setPath(path);
}

QVariant AttachmentItem::data(int column, int role) const {
    if (role == Qt::DecorationRole && column == 0) {
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Drive);
    }
    return TreeItem::data(column, role);
}

bool AttachmentItem::isAttachmentItem() {
    return true;
}

AttachmentItem::AttachmentItem(const QString & path, TreeItem *parentItem) : TreeItem({"Attachment"}, parentItem) {
    setPath(path);
}

QVariant WorkshopItem::data(int column, int role) const {
    if (role == Qt::DecorationRole && column == 0) {
        QFileIconProvider provider;
        return provider.icon(QFileIconProvider::Computer);
    }
    return TreeItem::data(column, role);
}

bool WorkshopItem::isWorkshopItem() {
    return true;
}

WorkshopItem::WorkshopItem(const QString & path, TreeItem *parentItem) : TreeItem({"Workshop"}, parentItem) {
    setPath(path);
}

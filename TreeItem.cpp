
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
        QFileIconProvider iconProvider;
        return iconProvider.icon(fileInfo);
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
    return !isAttachmentItem() && !isTrashItem() && !isWorkshopItem() && m_childItems.empty();
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

TrashItem::TrashItem(TreeItem *parentItem) : TreeItem({"Trash"}, parentItem) {

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

AttachmentItem::AttachmentItem(TreeItem *parentItem) : TreeItem({"Attachment"}, parentItem) {

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

WorkshopItem::WorkshopItem(TreeItem *parentItem) : TreeItem({"Workshop"}, parentItem) {

}

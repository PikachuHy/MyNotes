
#include "TreeModel.h"
#include "TreeItem.h"
#include "Constant.h"
#include <QStringList>
#include <filesystem>
#include <regex>
#include <QDir>

namespace fs = std::filesystem;

TreeModel::TreeModel(const QString &path, QObject *parent)
        : QAbstractItemModel(parent) {
    m_dataPath = path + "/" + Constant::workshop;
    ensurePathExist(path);
    ensurePathExist(path + "/" + Constant::workshop);
    ensurePathExist(path + "/" + Constant::attachment);
    ensurePathExist(path + "/" + Constant::trash);
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

    if (role != Qt::DisplayRole && role != Qt::DecorationRole)
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
    auto workshopItem = new WorkshopItem(parent);
    parent->appendChild(workshopItem);
    parent->appendChild(new AttachmentItem(parent));
    parent->appendChild(new TrashItem(parent));
    buildFileTree(m_dataPath, workshopItem);
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

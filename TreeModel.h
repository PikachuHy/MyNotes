
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "Settings.h"
class TreeItem;
class WatchingItem;
class DbManager;
class TreeModel : public QAbstractItemModel {
Q_OBJECT

public:
    explicit TreeModel(const QString &dataPath, DbManager* dbManager, QObject *parent = nullptr);

    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex addNewNode(const QModelIndex& parent, TreeItem* child);

    QModelIndex addNewFolder(const QModelIndex& parent, TreeItem* child);

    void removeNode(const QModelIndex& index);

    void addWatchingDir(const QString& path);
private:
    void setupModelData(TreeItem *parent);
    void buildFileTreeFromDb(int parentPathId, TreeItem* parentItem);
    void ensurePathExist(QString path);

    inline QString workshopPath();
    inline QString attachmentPath();
    inline QString trashPath();
private:
    TreeItem *rootItem;
    QString m_dataPath;
    DbManager* m_dbManager;
    Settings* m_settings;
    WatchingItem* m_watchingItem;
};

#endif // TREEMODEL_H

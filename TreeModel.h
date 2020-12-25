
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;

class TreeModel : public QAbstractItemModel {
Q_OBJECT

public:
    explicit TreeModel(const QString &data, QObject *parent = nullptr);

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
private:
    void setupModelData(TreeItem *parent);

    void ensurePathExist(QString path);

    inline QString workshopPath();
    inline QString attachmentPath();
    inline QString trashPath();
private:
    TreeItem *rootItem;
    QString m_dataPath;
};

#endif // TREEMODEL_H

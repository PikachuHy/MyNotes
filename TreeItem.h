
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>

class TreeItem {
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);

    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);

    int childCount() const;

    int columnCount() const;

    virtual QVariant data(int column, int role) const;

    int row() const;

    TreeItem *parentItem();

    void setPath(QString path);

    QString path();

    virtual bool isTrashItem();

    virtual bool isAttachmentItem();

    virtual bool isWorkshopItem();

    bool isFile();

private:
    QList<TreeItem *> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
    QString m_path;
};

class TrashItem : public TreeItem {
public:
    explicit TrashItem(TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isTrashItem() override;
};

class AttachmentItem : public TreeItem {
public:
    explicit AttachmentItem(TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isAttachmentItem() override;
};

class WorkshopItem : public TreeItem {
public:
    explicit WorkshopItem(TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isWorkshopItem() override;
};

#endif

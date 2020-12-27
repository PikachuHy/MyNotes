
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include "DbModel.h"
class TreeItem {
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);

    ~TreeItem();

    void appendChild(TreeItem *child);

    int insertFolder(TreeItem* child);

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

    virtual bool isFile();

    virtual int pathId();

    int removeChild(TreeItem* item);

    int indexInParent();
private:
    QList<TreeItem *> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
    QString m_path;
    bool m_isFolder;
};

class NoteItem: public TreeItem {
public:
    explicit NoteItem(Note note, TreeItem* parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isFile() override;

    int pathId() override;

private:
    Note m_note;
};

class FolderItem: public TreeItem {
public:
    explicit FolderItem(Path path, TreeItem* parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isFile() override;

    int pathId() override;

private:
    Path m_path;
};

class TrashItem : public TreeItem {
public:
    explicit TrashItem(const QString & path, TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isTrashItem() override;

    bool isFile() override;
};

class AttachmentItem : public TreeItem {
public:
    explicit AttachmentItem(const QString & path, TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isAttachmentItem() override;

    bool isFile() override;
};

class WorkshopItem : public TreeItem {
public:
    explicit WorkshopItem(const QString & path, TreeItem *parentItem = nullptr);

    QVariant data(int column, int role) const override;

    bool isWorkshopItem() override;

    bool isFile() override;
};

#endif

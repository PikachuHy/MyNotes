#ifndef TREEMODELADAPTOR_H
#define TREEMODELADAPTOR_H

#include <QObject>
#include <QList>
#include <QAbstractItemModel>
class TreeModelAdaptor: public QAbstractItemModel
{
  Q_OBJECT
  struct TreeItem;
public:
  explicit TreeModelAdaptor(QObject *parent = nullptr) : QAbstractItemModel(parent) {}

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QAbstractItemModel *model() const;
  void setModel(QAbstractItemModel *newModel);
private:
  int nextSibling(int n);
  bool isLastChild(int n);
  int itemIndex(const QModelIndex& index);
public:
  Q_INVOKABLE void expandRow(int n);
  Q_INVOKABLE void collapseRow(int n);
  Q_INVOKABLE bool isExpanded(int row) const;
signals:
  void modelChanged();

private:
  QAbstractItemModel* m_model;
  Q_PROPERTY(QAbstractItemModel * model READ model WRITE setModel NOTIFY modelChanged)

  enum {
    DepthRole = Qt::UserRole - 5,
    ExpandedRole,
    HasChildrenRole,
    HasSiblingRole,
    ModelIndexRole
  };

  struct TreeItem {
    QPersistentModelIndex index;
    int depth;
    bool expanded;

    explicit TreeItem(const QModelIndex &idx = QModelIndex(), int d = 0, int e = false)
        : index(idx), depth(d), expanded(e)
    { }

    inline bool operator== (const TreeItem &other) const
    {
      return this->index == other.index;
    }
  };
  QList<TreeItem> m_items;

};


#endif // TREEMODELADAPTOR_H

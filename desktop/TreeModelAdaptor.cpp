#include "TreeModelAdaptor.h"
#include <QDebug>


QModelIndex TreeModelAdaptor::index(int row, int column, const QModelIndex &parent) const
{
  // qDebug() << "index:" << row <<"," << column << parent.isValid();
#if 1
  if (column != 0) {
    qWarning() << "column is not 0";
    return QModelIndex();
  }
  if (row >= 0 && row < m_items.size()) {
    // qDebug() << "create index";
    return createIndex(row, column, (void*)&m_items[row]);
  }
  return QModelIndex();
#endif
}

#if 1
QModelIndex TreeModelAdaptor::parent(const QModelIndex &child) const
{
  //    qDebug() << "parent: " << child;
  return QModelIndex();
}
#endif
int TreeModelAdaptor::rowCount(const QModelIndex &parent) const
{
//    qDebug() << "row count" << parent;
  return m_items.size();
}

int TreeModelAdaptor::columnCount(const QModelIndex &parent) const
{
  //    qDebug() << "column count";
  if (!parent.isValid()) {
    return 0;
  }
  //    qDebug() << "column 1";
  return 1;
}

QVariant TreeModelAdaptor::data(const QModelIndex &index, int role) const
{
  //    qDebug() << "data" << index << role;
  if (!index.isValid()) {
    return QVariant();
  }


  switch (role) {
  case DepthRole:
    // qDebug() << "DepthRole";
    return m_items.at(index.row()).depth;
  case ExpandedRole:
    // qDebug() << "ExpandedRole";
    return m_items.at(index.row()).expanded;
  case HasChildrenRole:
    // qDebug() << "HasChildrenRole";
      return m_model->hasChildren(m_items[index.row()].index);
    //    case HasSiblingRole:
    //        return modelIndex.row() != m_model->rowCount(modelIndex.parent()) - 1;
  case ModelIndexRole:
    // qDebug() << "ModelIndexRole";
    return m_items.at(index.row()).index;
  default:
    if (role == Qt::DisplayRole) {
      // qDebug() << "DisplayRole";
    }
    auto ret =  m_model->data(m_items.at(index.row()).index, role);
    // qDebug() << "row:" << index.row() << "role:" << role << ret;
    return ret;
  }
}

QHash<int, QByteArray> TreeModelAdaptor::roleNames() const
{

  if (!m_model)
    return QHash<int, QByteArray>();

  QHash<int, QByteArray> modelRoleNames = m_model->roleNames();
  modelRoleNames.insert(DepthRole, "_q_TreeView_ItemDepth");
  modelRoleNames.insert(ExpandedRole, "_q_TreeView_ItemExpanded");
  modelRoleNames.insert(HasChildrenRole, "_q_TreeView_HasChildren");
  modelRoleNames.insert(HasSiblingRole, "_q_TreeView_HasSibling");
  modelRoleNames.insert(ModelIndexRole, "_q_TreeView_ModelIndex");
  qDebug() << modelRoleNames;
  return modelRoleNames;
}

QAbstractItemModel *TreeModelAdaptor::model() const
{
  return m_model;
}

void TreeModelAdaptor::setModel(QAbstractItemModel *newModel)
{
  if (m_model == newModel)
    return;
  m_model = newModel;
  emit modelChanged();

#if 1
  int childrenCount = m_model->rowCount();
  beginInsertRows(QModelIndex(), 0, childrenCount);
  int rowDepth = 0;
  auto index = QModelIndex();
  m_items.reserve(m_items.count() + childrenCount);
  for (int i = 0; i < childrenCount; i++) {
    const QModelIndex &cmi = m_model->index(i, 0, index);
    m_items.insert(i, TreeItem(cmi, rowDepth, false));
  }
  endInsertRows();
#endif
}

int TreeModelAdaptor::nextSibling(int n)
{
  if (n >= m_items.size()) {
    qWarning() << "next sibling of" << n;
    return n;
  }
  auto index = m_items[n].index;
  if (isLastChild(n)) {
    return nextSibling(itemIndex(index.parent()));
  }
  auto nextSiblingIndex = m_model->sibling(index.row() + 1, 0, index.parent());
  return itemIndex(nextSiblingIndex);
}

bool TreeModelAdaptor::isLastChild(int n)
{
  auto parentIndex = m_items[n].index.parent();
  int totalChild = m_model->rowCount(parentIndex);
  return totalChild == m_items[n].index.row() + 1;
}

int TreeModelAdaptor::itemIndex(const QModelIndex &index)
{
  for(int i=0;i<m_items.size();i++) {
    if (m_items[i].index == index) {
      return i;
    }
  }
  return m_items.size();
}

void TreeModelAdaptor::expandRow(int n)
{
    qDebug() << "expand row"<< n;
  if (n < 0 || n >= m_items.size()) {
    qDebug() << "invalid:"<<n;
    return;
  }
  if (m_items[n].expanded) {
    qDebug() << "row " << n << "already expanded";
    return;
  }
  int childrenCount = m_model->rowCount(m_items[n].index);
  qDebug() << "child count" << childrenCount;
  if (childrenCount == 0) {
    qDebug() << "no child";
    return;
  }

  int rowDepth = m_items[n].depth + 1;
  int startIdx = n + 1;
  auto index = m_items[n].index;
  // qDebug() << "insert row:" << startIdx << startIdx + childrenCount - 1;
  beginInsertRows(QModelIndex(), startIdx, startIdx + childrenCount - 1);
  m_items.reserve(m_items.count() + childrenCount);
  for (int i = 0; i < childrenCount; i++) {
    const QModelIndex &cmi = m_model->index(i, 0, index);
    qDebug() << m_model->data(index);
    m_items.insert(startIdx+i, TreeItem(cmi, rowDepth, false));
  }
  endInsertRows();
  m_items[n].expanded = true;
#if 0
    qDebug() << "all data in items";
    for(const auto& item: m_items) {
        qDebug() << m_model->data(item.index);
    }
#endif
}

void TreeModelAdaptor::collapseRow(int n)
{
  if (n < 0 || n >= m_items.size()) {
    qDebug() << "invalid:"<<n;
    return;
  }
  if (!m_items[n].expanded) {
    qDebug() << "row " << n << "not expanded";
    return;
  }
  int childrenCount = m_model->rowCount(m_items[n].index);
  if (childrenCount == 0) {
    return;
  }
  int startIdx = n + 1;
  // collapse时，需要把当前结点下所有展开的结点全部删掉
  // 也就是当前parentIndex != m_items[n].index.parent()的
  int endIdx = startIdx;
  while (endIdx < m_items.size() && m_items[endIdx].index.parent() != m_items[n].index.parent()) {
    endIdx++;
  }
  qDebug() << "remove"<< startIdx << endIdx;
  beginRemoveRows(QModelIndex(), startIdx, endIdx - 1);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  for (int i=startIdx;i<endIdx;i++) {
    m_items.removeAt(startIdx);
  }
#else
  m_items.remove(startIdx, endIdx - startIdx);
#endif
  endRemoveRows();
  m_items[n].expanded = false;
}

bool TreeModelAdaptor::isExpanded(int row) const
{
  if (row < 0 || row >= m_items.size()) {
    return false;
  }
  return m_items[row].expanded;
}

#include "TreeView.h"
#include <QWidget>
#include <QTreeView>
#include <QHeaderView>

TreeView::TreeView(QWidget *parent) : QTreeView(parent) {
    header()->hide();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

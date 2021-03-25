#include "TreeView.h"
#include <QWidget>
#include <QTreeView>
#include <QHeaderView>

TreeView::TreeView(QWidget *parent) : QTreeView(parent) {
    header()->hide();
    setContextMenuPolicy(Qt::CustomContextMenu);
#ifdef Q_OS_WIN
    // setRootIsDecorated(false);
    setFrameStyle(QFrame::NoFrame);
    // setStyleSheet("QTreeView::branch {image:none;}");
#endif
}

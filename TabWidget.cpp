//
// Created by pikachu on 2021/4/19.
//

#include "TabWidget.h"
#include "TextPreview.h"
#include <QEvent>
#include <QKeyEvent>
#include "TabBar.h"
#include <QMenu>
TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent) {
    auto tabBar = new TabBar();
    setTabBar(tabBar);
    installEventFilter(this);
    setMovable(true);
    setTabsClosable(true);
//    setTabShape(Triangular);

    connect(tabBar, &QTabBar::customContextMenuRequested, [this, tabBar](const QPoint &pos){
        qDebug() << pos;
        QMenu menu;
        // https://www.qtcentre.org/threads/16703-QTabBar-Context-menu-on-tab
        bool posInTab = false;
        int i;
        for (i = 0; i < tabBar->count(); ++i) {
            if (tabBar->tabRect(i).contains(pos)) {
                posInTab = true;
                break;
            }
        }
        if (posInTab) {
            menu.addAction(tr("Close"), [this, i]() {
                this->removeTab(i);
            });
            menu.addAction(tr("Close Other Tabs"), [this, i]() {
                auto tab = this->tabs()[i];
                this->clear();
                this->add(tab);
            });
            menu.addAction(tr("Close All Tabs"), [this]() {
                this->clear();
            });
        } else {

        }
        menu.exec(this->mapToGlobal(pos));
    });
}

void TabWidget::add(TextPreview *tab) {
    QTabWidget::addTab(tab, tab->fileName());
    setCurrentWidget(tab);
    setDocumentMode(true);
    setTabBarAutoHide(true);
}

QVector<TextPreview *> TabWidget::tabs() const {
    QVector<TextPreview*> tabs;

    for (int i = 0; i < count(); i++)
    {
        tabs.push_back(tabAt(i));
    }

    return tabs;
}

TextPreview *TabWidget::currentTab() const {
    return qobject_cast<TextPreview*>(widget(currentIndex()));
}

TextPreview *TabWidget::tabAt(int index) const {
    if (index < 0 || index >= count())
    {
        return nullptr;
    }

    return qobject_cast<TextPreview*>(widget(index));
}

bool TabWidget::eventFilter(QObject *obj, QEvent *event) {

    bool isKeyPress = event->type() == QEvent::KeyPress;

    if (isKeyPress)
    {
        QKeyEvent *keyInfo = static_cast<QKeyEvent*>(event);
        int key = keyInfo->key();

        if (keyInfo->modifiers() == Qt::ControlModifier)
        {
            // Ctrl + num = jump to that tab number
            if (key >= Qt::Key_1 && key <= Qt::Key_9)
            {
                setCurrentWidget(tabAt(key - Qt::Key_1));
                return true;
            }

                // Ctrl + tab = advance tab by one
            else if (key == Qt::Key_T)
            {
                int newTabIndex = (currentIndex() + 1) % count();
                setCurrentWidget(tabAt(newTabIndex));
                return true;
            }
        }
        else
        {
            return QObject::eventFilter(obj, event);
        }
    }

    return QObject::eventFilter(obj, event);
}

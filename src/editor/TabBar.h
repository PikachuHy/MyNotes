//
// Created by pikachu on 2021/4/19.
//

#ifndef MYNOTES_TABBAR_H
#define MYNOTES_TABBAR_H
#include <QObject>
#include <QTabBar>
class TabBar : public QTabBar {
Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);
private:
};


#endif //MYNOTES_TABBAR_H

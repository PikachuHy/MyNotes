//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_LISTVIEW_H
#define MYNOTES_LISTVIEW_H
#include <QListView>

class ListView : public QListView {
Q_OBJECT
public:
    explicit ListView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};


#endif //MYNOTES_LISTVIEW_H

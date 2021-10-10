//
// Created by pikachu on 5/23/2021.
//

#ifndef MYNOTES_KEYFILTER_H
#define MYNOTES_KEYFILTER_H

#include <QObject>

class KeyFilter : public QObject {
Q_OBJECT
public:
    void setFilter(QObject *o);

signals:

    void keyBackPress();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
};


#endif //MYNOTES_KEYFILTER_H

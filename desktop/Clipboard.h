//
// Created by pikachu on 2021/6/24.
//

#ifndef MYNOTES_CLIPBOARD_H
#define MYNOTES_CLIPBOARD_H

#include <QObject>
class Clipboard : public QObject {
Q_OBJECT
public:
    Q_INVOKABLE void copyText(QString text);
private:
};


#endif //MYNOTES_CLIPBOARD_H

//
// Created by PikachuHy on 2021/3/24.
//

#ifndef MYNOTES_ELASTICSEARCHRESTAPI_H
#define MYNOTES_ELASTICSEARCHRESTAPI_H

#include <QString>
#include <QObject>
#include "DbModel.h"
class ElasticSearchRestApi: public QObject {
    Q_OBJECT
public:
    explicit ElasticSearchRestApi(QObject* parent = nullptr): QObject(parent) {}
    void putNote(const QString &owner, const QString &noteHtml, const Note& note);
    void handleResult(const QByteArray& ret);
};


#endif //MYNOTES_ELASTICSEARCHRESTAPI_H

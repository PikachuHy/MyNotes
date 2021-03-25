//
// Created by PikachuHy on 2021/3/24.
//

#ifndef MYNOTES_ELASTICSEARCHRESTAPI_H
#define MYNOTES_ELASTICSEARCHRESTAPI_H

#include <QString>
#include <QObject>
#include "DbModel.h"
struct SearchResult {
    QString strId;
    QString note;
    QString highlight;
};
class ElasticSearchRestApi: public QObject {
    Q_OBJECT
public:
    explicit ElasticSearchRestApi(QObject* parent = nullptr);
    void putNote(const QString &owner, const QString &noteHtml, const Note& note);
    void handleResult(const QByteArray& ret);
    QList<SearchResult> search(const QString& q);
    QByteArray buildSearchJson(const QString& q);
private:
    QString baseUrl;
};


#endif //MYNOTES_ELASTICSEARCHRESTAPI_H

//
// Created by PikachuHy on 2021/3/24.
//

#ifndef MYNOTES_ELASTICSEARCHRESTAPI_H
#define MYNOTES_ELASTICSEARCHRESTAPI_H

#include <QString>
#include <QObject>
#include "DbModel.h"
class Settings;
struct SearchResult {
    QString strId;
    QString note;
    QString highlight;
};
struct ServerNoteInfo {
    QString strId;
    QString title;
    QString noteHtml;
    QString owner;

};
class ElasticSearchRestApi: public QObject {
    Q_OBJECT
public:
    explicit ElasticSearchRestApi(QObject* parent = nullptr);
    void putNote(const QString &owner, const QString &noteHtml, const Note& note);
    void putNote(const ServerNoteInfo& info);
    void handleResult(const QByteArray& ret);
    QList<SearchResult> search(const QString& q);
    QByteArray buildSearchJson(const QString& q);
private:
    QString baseUrl;
    Settings* m_settings;
};


#endif //MYNOTES_ELASTICSEARCHRESTAPI_H

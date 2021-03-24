//
// Created by PikachuHy on 2021/3/24.
//

#include "ElasticSearchRestApi.h"
#include <QCoreApplication>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <iostream>
#include <QDebug>
#include <QEventLoop>
#include <QObject>
void ElasticSearchRestApi::putNote(const QString &owner, const QString &noteHtml, const Note& note) {

    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request;
    QByteArray line;
    QEventLoop eventLoop;
    QString url = QString("http://192.168.3.100:9200/note/_doc/") + note.strId();
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QJsonObject jsonObject;
    jsonObject.insert("owner", owner);
    jsonObject.insert("note", noteHtml);
    QJsonDocument document;
    document.setObject(jsonObject);
    QByteArray append = document.toJson(QJsonDocument::Compact);
//    QByteArray append(R"({"user":"david","token":"e10adc3949ba59abbe56e057f20f883e"})");
    qDebug() << url;
    qDebug().noquote() << append;
    QNetworkReply* reply = manager->put(request, append);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    line=reply->readAll();
    handleResult(line);
}
/*
{
  "_index" : "note",
  "_type" : "_doc",
  "_id" : "666",
  "_version" : 1,
  "result" : "created",
  "_shards" : {
    "total" : 2,
    "successful" : 1,
    "failed" : 0
  },
  "_seq_no" : 6,
  "_primary_term" : 1
}

 */
void ElasticSearchRestApi::handleResult(const QByteArray &ret) {
    qDebug().noquote() << ret;
    QJsonParseError jsonError;
// 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(ret, &jsonError);
// 解析未发生错误
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {
            // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("_shards")) {
                auto shardsObject = object.value("_shards").toObject();
                // 包含指定的 key
                QJsonValue value = shardsObject.value("successful");  // 获取指定 key 对应的 value
                qDebug() << value;
                if (value.toInt()) {
                    qDebug() << "update success";
                } else {
                    qDebug() << "update fail";
                }
            }
        }
    }

}

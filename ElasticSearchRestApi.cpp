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
#include <QJsonArray>
#include <Settings.h>
void ElasticSearchRestApi::putNote(const QString &owner, const QString &noteHtml, const Note& note) {

    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request;
    QByteArray line;
    QEventLoop eventLoop;
    QString url = baseUrl + QString("/note/_doc/") + note.strId();
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QJsonObject jsonObject;
    jsonObject.insert("owner", owner);
    jsonObject.insert("note", noteHtml);
    jsonObject.insert("title", note.title());
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

QList<SearchResult> ElasticSearchRestApi::search(const QString &q) {
    QList<SearchResult> ret;

    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request;
    QByteArray line;
    QEventLoop eventLoop;
    QString url = baseUrl + QString("/note/_search");
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QByteArray append = buildSearchJson(q);
//    QByteArray append(R"({"user":"david","token":"e10adc3949ba59abbe56e057f20f883e"})");
    qDebug() << url;
    qDebug().noquote() << append;
    // 强行POST
    QNetworkReply* reply = manager->post(request, append);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    line=reply->readAll();

    qDebug().noquote() << line;
    QJsonParseError jsonError;
// 转化为 JSON 文档
    QJsonDocument doucment = QJsonDocument::fromJson(line, &jsonError);
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
                    qDebug() << "search success";
                } else {
                    qDebug() << "search fail";
                }
            }
            if (object.contains("hits")) {
                // TODO 构造查询结果
            }
        }
    }
    return ret;
}

ElasticSearchRestApi::ElasticSearchRestApi(QObject *parent) : QObject(parent), m_settings(Settings::instance()) {
    QString serverIp = Settings::instance()->serverIp;
    baseUrl = "http://" + serverIp + ":9200";
}

QByteArray ElasticSearchRestApi::buildSearchJson(const QString& q) {
    QJsonObject jsonObject;
    QJsonObject matchObject;
    matchObject.insert("note", q);
    QJsonObject queryObject;
    queryObject.insert("match", matchObject);
    jsonObject.insert("query", queryObject);
    QJsonObject highlightObject;
    highlightObject.insert("pre_tags", "<span style='color:red'>");
    highlightObject.insert("post_tags", "</span>");
    QJsonObject fieldsObject;
    QJsonObject noteObject;
    fieldsObject.insert("note", noteObject);
    highlightObject.insert("fields", fieldsObject);
    jsonObject.insert("highlight", highlightObject);
    QJsonArray sourceArray;
    sourceArray.append("title");
    sourceArray.append("note");
    jsonObject.insert("_source", sourceArray);
    jsonObject.insert("from", 0);
    jsonObject.insert("size", 10);
    QJsonDocument document;
    document.setObject(jsonObject);
    return document.toJson(QJsonDocument::Compact);
}

void ElasticSearchRestApi::putNote(const ServerNoteInfo& info) {

    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request;
    QByteArray line;
    QEventLoop eventLoop;
    QString url = baseUrl + QString("/note/_doc/") + info.strId;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QJsonObject jsonObject;
    jsonObject.insert("owner", info.owner);
    jsonObject.insert("note", info.noteHtml);
    jsonObject.insert("title", info.title);
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

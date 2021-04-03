//
// Created by PikachuHy on 2021/3/25.
//

#include "Http.h"
#include <QUrl>
QByteArray Http::get(const QString &url) {
    qDebug() << "GET:" << url;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    request.setUrl(url);
    auto reply = manager.get(request);
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

QByteArray Http::uploadFile(const QString &url, QFile &file) {
    qDebug() << "upload file:" << file.fileName();
    qDebug() << "POST:" << url;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/octet-stream"));
    file.open(QIODevice::ReadOnly);
    int file_len = file.size();
    qDebug() << "file len:" << file_len;
    QDataStream in(&file);
    auto buffer = new char[file_len];
    in.readRawData(buffer, file_len);
    file.close();
    QByteArray dataArray = QByteArray(buffer, file_len);
    auto reply = manager.post(request, dataArray);
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

Http *Http::instance() {
    static Http singleton;
    return &singleton;
}

Http::Http() {

}

QByteArray Http::uploadFile(const QString &url, const QByteArray &dataArray) {
    qDebug() << "upload file by QByteArray";
    qDebug() << "POST:" << url;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/octet-stream"));
    auto reply = manager.post(request, dataArray);
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

QByteArray Http::post(const QString &url, const QByteArray& body) {
    qDebug() << "POST:" << url;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QNetworkAccessManager manager;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setUrl(url);
    auto reply = manager.post(request, body);
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

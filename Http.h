//
// Created by PikachuHy on 2021/3/25.
//

#ifndef MYNOTES_HTTP_H
#define MYNOTES_HTTP_H

#include <QApplication>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QDir>
#include <QDebug>
#include <QWidget>

class Http {

public:
    static Http *instance();
    QByteArray get(const QString& url);
    QByteArray post(const QString& url, const QByteArray& body);
    QByteArray uploadFile(const QString& url, QFile &file);
    QByteArray uploadFile(const QString& url, const QByteArray& dataArray);
private:
    Http();
};


#endif //MYNOTES_HTTP_H

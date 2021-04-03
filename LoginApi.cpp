//
// Created by PikachuHy on 2021/4/3.
//
#include "LoginApi.h"
#include "Http.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

LoginResult LoginApi::login(LoginParam &param) {
    LoginResult ret;
    ret.success = false;
    if (param.baseUrl.isEmpty()) {
        auto msg = "server url is empty";
        ret.msg = msg;
        return ret;
    }
    if (param.account.isEmpty()) {
        auto msg = "account is empty";
        ret.msg = msg;
        return ret;
    }
    if (param.password.isEmpty()) {
        auto msg = "password is empty";
        ret.msg = msg;
        return ret;
    }
    QUrlQuery urlQuery;
    QString url = param.baseUrl;
    urlQuery.addQueryItem("account", param.account);
    urlQuery.addQueryItem("password", param.password);
    auto res = Http::instance()->post(url, urlQuery.query().toUtf8());
    auto doc = QJsonDocument::fromJson(res);
    if (doc.isNull()) {
        ret.msg = res;
    } else {
        ret.success = true;
        auto o = doc.object();
        ret.usernameEn = o.value("name_en").toString();
        ret.usernameZh = o.value("name_zh").toString();
    }
    return ret;
}

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
    QJsonDocument paramDoc;
    QJsonObject paramObject;
    paramObject.insert("account", param.account);
    paramObject.insert("password", param.password);
    paramDoc.setObject(paramObject);
    auto res = Http::instance()->postJSON(param.baseUrl, paramDoc.toJson());
    auto doc = QJsonDocument::fromJson(res);
    if (doc.isNull()) {
        ret.msg = res;
    } else {
        auto o = doc.object();
        bool success = o.value("success").toBool(false);
        if (success) {
            QJsonObject dataObject = o.value("data").toObject();
            ret.usernameEn = dataObject.value("name_en").toString();
            ret.usernameZh = dataObject.value("name_zh").toString();
        } else {
            ret.msg = o.value("data").toString();
        }
        ret.success = success;
    }
    return ret;
}

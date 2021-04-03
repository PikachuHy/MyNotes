//
// Created by PikachuHy on 2021/4/3.
//

#ifndef MYNOTES_LOGINAPI_H
#define MYNOTES_LOGINAPI_H

#include <QObject>
#include "Singleton.h"
struct LoginParam {
    QString baseUrl;
    QString account;
    QString password;
};

struct LoginResult {
    bool success;
    QString usernameZh;
    QString usernameEn;
    QString msg;
};

class LoginApi : public Singleton<LoginApi>{
public:
    LoginResult login(LoginParam& param);
};


#endif //MYNOTES_LOGINAPI_H

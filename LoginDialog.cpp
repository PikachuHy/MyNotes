//
// Created by PikachuHy on 2021/4/3.
//

#include "LoginDialog.h"
#include <QIcon>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include "Settings.h"
#include "LoginApi.h"
LoginDialog::LoginDialog() {
    setWindowIcon(QIcon(":/icon/notebook_128x128.png"));
    auto layout = new QVBoxLayout();
    {
        auto hbox = new QHBoxLayout();
        hbox->addStretch(1);
        auto icon = new QLabel();
        icon->setPixmap(QPixmap(":/icon/notebook_128x128.png"));
        hbox->addWidget(icon);
        hbox->addStretch(1);
        layout->addLayout(hbox);
    }
    auto loginForm = new QFormLayout();
    auto accountLineEdit = new QLineEdit();
    auto passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginForm->addRow(tr("Account:"), accountLineEdit);
    loginForm->addRow(tr("Password:"), passwordLineEdit);
    layout->addLayout(loginForm);
    auto rememberPasswordCheckBox = new QCheckBox(tr("Remember password"));
    auto autoLoginCheckBox = new QCheckBox(tr("Auto login"));
    loginForm->addRow(rememberPasswordCheckBox);
    loginForm->addRow(autoLoginCheckBox);
    auto loginBtn = new QPushButton(tr("Login"));
    layout->addWidget(loginBtn);
    auto justTryBtn = new QPushButton(tr("Just try"));
    layout->addWidget(justTryBtn);
    connect(loginBtn, &QPushButton::clicked,
            [this, accountLineEdit, passwordLineEdit](){
        auto account = accountLineEdit->text();
        auto password = passwordLineEdit->text();
        if (account.isEmpty()) {
            showWarning(tr("Login"), tr("Account can't be empty."));
            return;
        }
        if (password.isEmpty()) {
            showWarning(tr("Login"), tr("Password can't be empty."));
            return;
        }
        LoginParam param;
        param.baseUrl = "http://in.css518.cn:9201/login";
        param.account = account;
        param.password = password;
        auto ret = LoginApi::instance()->login(param);
        if (ret.success) {
            Settings::instance()->usernameEn = ret.usernameEn;
            Settings::instance()->usernameZh = ret.usernameZh;
            this->accept();
        } else {
            showError(tr("Login fail"), ret.msg);
        }
    });
    connect(justTryBtn, &QPushButton::clicked, [this]() {
        showWarning(tr("Just try"), tr("This feature is still under development."));
    });
    setLayout(layout);
}

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
#include "Settings.h"
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
    connect(loginBtn, &QPushButton::clicked, [this](){

    });
    connect(justTryBtn, &QPushButton::clicked, [this]() {

    });
    setLayout(layout);
}

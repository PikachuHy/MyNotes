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
#include <QTimer>
#include <QDebug>
#include <QMovie>
#include "Settings.h"
#include "LoginApi.h"
#include "Utils.h"


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
    m_accountLineEdit = new QLineEdit();
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_accountLineEdit->setText(Settings::instance()->userAccount);
    m_passwordLineEdit->setText(Settings::instance()->userPassword);
    loginForm->addRow(tr("Account:"), m_accountLineEdit);
    loginForm->addRow(tr("Password:"), m_passwordLineEdit);
    layout->addLayout(loginForm);
    m_rememberPasswordCheckBox = new QCheckBox(tr("Remember password"));
    m_autoLoginCheckBox = new QCheckBox(tr("Auto login"));
    qDebug() << Settings::instance()->userRememberPassword;
    qDebug() << Settings::instance()->userAutoLogin;
    m_rememberPasswordCheckBox->setChecked(Settings::instance()->userRememberPassword);
    m_autoLoginCheckBox->setChecked(Settings::instance()->userAutoLogin);
    loginForm->addRow(m_rememberPasswordCheckBox);
    loginForm->addRow(m_autoLoginCheckBox);
    auto loginBtn = new QPushButton(tr("Login"));
    layout->addWidget(loginBtn);
    // 登录中的加载动画
    auto movie = new QMovie(":/icon/loading.gif");
    auto ok = movie->isValid();
    qDebug() << "movie ok?"<<ok;
    movie->start();
    auto loadingLabel = new QLabel(loginBtn);
    loadingLabel->setMovie(movie);
    loadingLabel->hide();
    auto justTryBtn = new QPushButton(tr("Just try"));
    layout->addWidget(justTryBtn);
    connect(m_rememberPasswordCheckBox, &QCheckBox::stateChanged, [this]() {
        qDebug() << "pass change";
        updateSettings();
    });
    connect(m_autoLoginCheckBox, &QCheckBox::stateChanged, [this]() {
        updateSettings();
    });

    connect(loginBtn, &QPushButton::clicked,[this, loginBtn, loadingLabel, movie](){
        updateSettings();
        auto account = m_accountLineEdit->text();
        auto password = m_passwordLineEdit->text();
        if (account.isEmpty()) {
            showWarning(tr("Login"), tr("Account can't be empty."));
            return;
        }
        Settings::instance()->userAccount = account;
        if (password.isEmpty()) {
            showWarning(tr("Login"), tr("Password can't be empty."));
            return;
        }
        if (Settings::instance()->userRememberPassword) {
            Settings::instance()->userPassword = password;
        }
        loadingLabel->show();
        auto loadingGeometry = loadingLabel->geometry();
        auto loginBtnGemetry = loginBtn->geometry();
        int centerX = (loginBtnGemetry.width() - loadingGeometry.width()) / 2;
        loadingLabel->move(centerX, loadingGeometry.y());
        loginBtn->setEnabled(false);
        // 强行让用户看看加载动画，哈哈哈
        QTimer::singleShot(1000, [this, loginBtn, loadingLabel, account, password]() {
            this->login(account, password);
            loadingLabel->hide();
            loginBtn->setEnabled(true);
        });

    });
    connect(justTryBtn, &QPushButton::clicked, [this]() {
        showWarning(tr("Just try"), tr("This feature is still under development."));
    });
    setLayout(layout);

    if (Settings::instance()->userAutoLogin) {
        qDebug() << "auto login";
        QString account = Settings::instance()->userAccount;
        QString password = Settings::instance()->userPassword;
        if (!account.isEmpty() && !password.isEmpty()) {
            QTimer::singleShot(1000, [this, account, password](){
                this->login(account, password);
            });
        }
    }
}

void LoginDialog::login(const QString &account, const QString &password) {
    LoginParam param;
    param.baseUrl = "http://in.css518.cn:9201/login";
    param.account = account;
    param.password = password;
    auto ret = LoginApi::instance()->login(param);
    if (ret.success) {
        Settings::instance()->usernameEn = ret.usernameEn;
        Settings::instance()->usernameZh = ret.usernameZh;
        QString infoStr = account + '.' + password + '.'
                          + ret.usernameZh + '.' + ret.usernameEn + ".MyNotes";
        Settings::instance()->userSignature = Utils::md5(infoStr);
        this->accept();
    } else {
        showError(tr("Login fail"), ret.msg);
    }
}

void LoginDialog::updateSettings() {
    auto rememberPassword = m_rememberPasswordCheckBox->isChecked();
    auto autoLogin = m_autoLoginCheckBox->isChecked();
    qDebug() << "update" << rememberPassword << autoLogin;
    Settings::instance()->userRememberPassword = rememberPassword;
    Settings::instance()->userAutoLogin = autoLogin;
}

//
// Created by PikachuHy on 2021/3/25.
//

#include "SettingsDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
SettingsDialog::SettingsDialog(QWidget *parent): QDialog(parent), m_settings(Settings::instance()) {
    setWindowTitle(tr("MyNotes Settings"));
    auto _font = font();
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(QPixmap(":/icon/settings_16x16.png")));
    _font.setPointSize(12);
    _font.setFamily("微软雅黑");
#else
    setWindowIcon(QIcon(QPixmap(":/icon/notebook_128x128.png")));
    _font.setPointSize(16);
#endif
    setFont(_font);
    auto layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Server")), 0, 0);
    layout->addWidget(new QLabel(tr("Account")), 1, 0);
    layout->addWidget(new QLabel(tr("Password")), 2, 0);
#ifdef Q_OS_WIN
    layout->addWidget(new QLabel(tr("Typora path")), 4, 0);
#endif
    m_baseUrlLineEdit = new QLineEdit();
    m_accountLineEdit = new QLineEdit();
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_typoraPathLineEdit = new QLineEdit();
    m_typoraPathChooseBtn = new QPushButton(tr("..."));
    layout->addWidget(m_baseUrlLineEdit, 0, 1, 1, 2);
    layout->addWidget(m_accountLineEdit, 1, 1, 1, 2);
    layout->addWidget(m_passwordLineEdit, 2, 1, 1, 2);
#ifdef Q_OS_WIN
    layout->addWidget(m_typoraPathLineEdit, 4, 1, 1, 1);
    layout->addWidget(m_typoraPathChooseBtn, 4, 2);
#endif
    m_baseUrlLineEdit->setText(Settings::instance()->serverIp);
    m_accountLineEdit->setText(Settings::instance()->userAccount);
    m_passwordLineEdit->setText(Settings::instance()->userPassword);
#ifdef Q_OS_WIN
    m_typoraPathLineEdit->setText(m_settings->value("path.typora").toString());
#endif
    m_confirmBtn = new QPushButton(tr("Confirm"));
    m_cancelBtn = new QPushButton(tr("Cancel"));
    auto hbox = new QHBoxLayout();
    hbox->addStretch(1);
    hbox->addWidget(m_confirmBtn);
    hbox->addWidget(m_cancelBtn);
#ifdef Q_OS_WIN
    layout->addLayout(hbox, 5, 0, 1, 3);
#else
    layout->addLayout(hbox, 4, 0, 1, 3);
#endif
    setLayout(layout);
    setMinimumWidth(400);
    adjustSize();
    QTimer::singleShot(50, [this](){
        auto screenSize = QApplication::primaryScreen()->size();
        auto x = (screenSize.width() - this->width()) / 2;
        auto y = (screenSize.height() - this->height()) / 2;
        this->move(x, y);
    });
    connect(m_typoraPathChooseBtn, &QPushButton::clicked, [this]() {
        auto path = QFileDialog::getOpenFileName(this,
                                     tr("Choose Typora Path"),
                                     QDir::homePath()

                                     );
        qDebug() << "choose path:" << path;
        this->m_typoraPathLineEdit->setText(path);
    });
    connect(m_confirmBtn, &QPushButton::pressed, [this](){
        auto warning = [this](const QString& msg) {
            QMessageBox::warning(this, tr("SettingsDialog"), msg);
        };
        auto server = this->m_baseUrlLineEdit->text();
        auto account = this->m_accountLineEdit->text();
        auto password = this->m_passwordLineEdit->text();
        auto typoraPath = this->m_typoraPathLineEdit->text();
        if (server.isEmpty()) {
            warning(tr("Server can't be empty"));
            return ;
        }
        if (account.isEmpty()) {
            warning(tr("Account can't be empty"));
            return ;
        }
        if (password.isEmpty()) {
            warning(tr("Password can't be empty"));
            return ;
        }
#ifdef Q_OS_WIN
        if (typoraPath.isEmpty()) {
            warning(tr("Typora path can't be empty"));
            return ;
        }
#endif
        Settings::instance()->serverIp = server;
        Settings::instance()->userAccount = account;
        Settings::instance()->userPassword = password;
#ifdef Q_OS_WIN
        Settings::instance()->typoraPath = typoraPath;
#endif
        this->accept();
    });
    connect(m_cancelBtn, &QPushButton::clicked, [this]() {
        this->reject();
    });

}

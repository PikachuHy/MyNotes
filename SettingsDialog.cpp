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
    auto layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("server.base_url")), 0, 0);
    layout->addWidget(new QLabel(tr("server.port")), 1, 0);
    layout->addWidget(new QLabel(tr("server.owner")), 2, 0);
    layout->addWidget(new QLabel(tr("server.password")), 3, 0);
#ifdef Q_OS_WIN
    layout->addWidget(new QLabel(tr("Typora path")), 4, 0);
#endif
    m_baseUrlLineEdit = new QLineEdit();
    m_ownerLineEdit = new QLineEdit();
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_portSpinBox = new QSpinBox();
    m_portSpinBox->setMinimum(0);
    m_portSpinBox->setMaximum(65535);
    m_typoraPathLineEdit = new QLineEdit();
    m_typoraPathChooseBtn = new QPushButton(tr("..."));
    layout->addWidget(m_baseUrlLineEdit, 0, 1, 1, 2);
    layout->addWidget(m_portSpinBox, 1, 1, 1, 2);
    layout->addWidget(m_ownerLineEdit, 2, 1, 1, 2);
    layout->addWidget(m_passwordLineEdit, 3, 1, 1, 2);
#ifdef Q_OS_WIN
    layout->addWidget(m_typoraPathLineEdit, 4, 1, 1, 1);
    layout->addWidget(m_typoraPathChooseBtn, 4, 2);
#endif
    m_baseUrlLineEdit->setText(m_settings->value("server.base_url").toString());
    m_portSpinBox->setValue(m_settings->value("server.port").toInt());
    m_ownerLineEdit->setText(m_settings->value("server.owner").toString());
    m_passwordLineEdit->setText(m_settings->value("server.password").toString());
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
        auto baseUrl = this->m_baseUrlLineEdit->text();
        auto port = this->m_portSpinBox->value();
        auto owner = this->m_ownerLineEdit->text();
        auto password = this->m_passwordLineEdit->text();
        auto typoraPath = this->m_typoraPathLineEdit->text();
        if (baseUrl.isEmpty()) {
            warning(tr("server.base_url can't be empty"));
            return ;
        }
        if (owner.isEmpty()) {
            warning(tr("server.owner can't be empty"));
            return ;
        }
        if (password.isEmpty()) {
            warning(tr("server.password can't be empty"));
            return ;
        }
#ifdef Q_OS_WIN
        if (typoraPath.isEmpty()) {
            warning(tr("Typora path can't be empty"));
            return ;
        }
#endif
        this->m_settings->setValue("server.base_url", baseUrl);
        this->m_settings->setValue("server.port", port);
        this->m_settings->setValue("server.owner", owner);
        this->m_settings->setValue("server.password", password);
#ifdef Q_OS_WIN
        this->m_settings->setValue("path.typora", typoraPath);
#endif
        this->accept();
    });
    connect(m_cancelBtn, &QPushButton::clicked, [this]() {
        this->reject();
    });

}

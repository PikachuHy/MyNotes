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
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QFileInfo>

SettingsDialog::SettingsDialog(QWidget *parent) : PiDialog(parent) {
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
    auto layout = new QVBoxLayout();
    auto formLayout = new QFormLayout();
    auto autoSyncCheckBox = new QCheckBox(tr("Auto sync"));
    formLayout->addRow(autoSyncCheckBox);
    autoSyncCheckBox->setChecked(Settings::instance()->syncAuto);
#ifdef Q_OS_WIN
    m_typoraPathLineEdit = new QLineEdit();
    m_typoraPathChooseBtn = new QPushButton(tr("..."));
    {
        auto hbox = new QHBoxLayout();
        hbox->addWidget(m_typoraPathLineEdit, 1);
        hbox->addWidget(m_typoraPathChooseBtn);
        formLayout->addRow(tr("Typora path:"), hbox);
    }
    {
        QString typoraPath = Settings::instance()->typoraPath;
        if (typoraPath.isEmpty()) {
            // 尝试几个通用的位置
            QStringList pathList;
            pathList << "C:\\Program Files\\Typora\\Typora.exe";
            pathList << "C:\\Program Files (x86)\\Typora\\Typora.exe";
            for(const auto& path: pathList) {
                QFileInfo info(path);
                if (info.exists() && info.isExecutable()) {
                    Settings::instance()->typoraPath = path;
                }
            }
        }
    }
    m_typoraPathLineEdit->setText(Settings::instance()->typoraPath);
#endif
    layout->addLayout(formLayout);
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(30, 240, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, [this]() {
#ifdef Q_OS_WIN
                         auto typoraPath = m_typoraPathLineEdit->text();
                         if (typoraPath.isEmpty()) {
                             showWarning(tr("SettingsDialog"), tr("Typora path can't be empty"));
                             return;
                         }
                         Settings::instance()->typoraPath = typoraPath;
#endif
                         this->accept();
                     }
    );
    layout->addWidget(buttonBox);
    setLayout(layout);
    setMinimumWidth(400);
    adjustSize();
    QTimer::singleShot(50, [this]() {
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
}

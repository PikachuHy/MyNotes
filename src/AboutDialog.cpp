//
// Created by PikachuHy on 2021/4/3.
//

#include "AboutDialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setMaximumWidth(500);
    auto layout = new QVBoxLayout();
    auto icon = new QLabel();
    icon->setPixmap(QPixmap(":icon/notebook_128x128.png"));
    layout->addWidget(icon);
    QString html;
    html.append(QString("<p>%1 v%2").arg(QApplication::applicationName())
                        .arg(QApplication::applicationVersion()));
    html.append(R"(<p>
MyNotes is an open-sourced project,
published under GPLv3 for individual/personal users and
custom commercial license for company users.

The project is based on Qt,
aimed to provide an excellent PKM(personal knowledge management) desktop environment.
</p>)");
    html.append(tr("<p>Design By PikachuHy<p>"));
    html.append(R"(
<a href="https://github.com/PikachuHy/MyNotes">
<img src=":icon/github_128x128.png"  width=64 height=64>
</a>
<a href="http://blog.pikachu.net.cn">
<img src=":icon/avatar_300x300.png" width=64 height=64>
</a>
)");
    auto content = new QLabel();
    content->setWordWrap(true);
    content->setText(html);
    connect(content, &QLabel::linkActivated, [this](const QString &link) {
        QDesktopServices::openUrl(QUrl(link));
    });
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(30, 240, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &AboutDialog::accept);

    layout->addWidget(content);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

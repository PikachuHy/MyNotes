//
// Created by PikachuHy on 2021/3/9.
//

#include "Toast.h"

#include <QApplication>


#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QTimer>

Toast::Toast(QWidget *parent)
        : QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);// 无边框 无任务栏
    setAttribute(Qt::WA_TranslucentBackground, true);   // 背景透明
    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet(R"(
background-color: rgba(0,0,0,0.80);
border-radius: 26px;
color: #FFFFFF;
font-size: 16px;
padding-left:25px;
padding-right:25px;
)");
    auto layout = new QHBoxLayout();
    layout->addWidget(m_label);
    setLayout(layout);
    m_label->setMinimumSize(0, 52);
}

Toast::~Toast()
{
}

void Toast::setText(const QString& text)
{
    m_label->setText(text);
}

void Toast::showAnimation(int timeout /*= 2000*/)
{
    // 开始动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    show();

    QTimer::singleShot(timeout, [&]
    {
        // 结束动画
        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(1000);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start();
        connect(animation, &QPropertyAnimation::finished, [&]
        {
            close();
            deleteLater();// 关闭后析构
        });
    });
}

void Toast::showTip(const QString& text, QWidget* parent /*= nullptr*/)
{
    Toast* toast = new Toast(parent);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint); // 置顶
    toast->setText(text);
    toast->adjustSize();    //设置完文本后调整下大小

    // 测试显示位于主屏的70%高度位置
    QScreen* pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, pScreen->size().height() * 7 / 10);
    toast->showAnimation();
}

void Toast::paintEvent(QPaintEvent *event)
{
    QPainter paint;
    paint.begin(this);
    auto kBackgroundColor = QColor(255, 255, 255);
    kBackgroundColor.setAlpha(0.0 * 255);// 透明度为0
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen(Qt::NoPen);
    paint.setBrush(QBrush(kBackgroundColor, Qt::SolidPattern));//设置画刷形式
    paint.drawRect(0, 0, width(), height());
    paint.end();
}

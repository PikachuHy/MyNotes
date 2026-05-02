//
// Created by PikachuHy on 2021/3/9.
//
/*
background-color: rgba(0,0,0,0.80);
border-radius: 26px;
color: #FFFFFF;
font-family: microsoft yahei;
font-size: 16px;
padding-left:25px;
padding-right:25px;
 */
#ifndef MYNOTES_TOAST_H
#define MYNOTES_TOAST_H

#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class Toast : public QWidget
{
Q_OBJECT

public:
    Toast(QWidget *parent = Q_NULLPTR);
    ~Toast();

    void setText(const QString& text);

    void showAnimation(int timeout = 2000);// 动画方式show出，默认2秒后消失

public:
    // 静态调用
    static void showTip(const QString& text, QWidget* parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QLabel *m_label;
};


#endif //MYNOTES_TOAST_H

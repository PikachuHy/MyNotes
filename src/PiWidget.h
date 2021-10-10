//
// Created by PikachuHy on 2021/4/3.
//

#ifndef MYNOTES_PIWIDGET_H
#define MYNOTES_PIWIDGET_H

#include <QWidget>

class PiWidget : public QWidget {
    Q_OBJECT
public:
    explicit PiWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void showInfo(const QString& title, const QString& msg);
    void showWarning(const QString& title, const QString& msg);
    void showError(const QString& title, const QString& msg);
private:
};


#endif //MYNOTES_PIWIDGET_H

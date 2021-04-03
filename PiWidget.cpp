//
// Created by PikachuHy on 2021/4/3.
//

#include "PiWidget.h"
#include <QMessageBox>
PiWidget::PiWidget(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {

}

void PiWidget::showInfo(const QString &title, const QString &msg) {
    QMessageBox::information(this, title, msg);
}

void PiWidget::showWarning(const QString &title, const QString &msg) {
    QMessageBox::warning(this, title, msg);
}

void PiWidget::showError(const QString &title, const QString &msg) {
    QMessageBox::critical(this, title, msg);
}

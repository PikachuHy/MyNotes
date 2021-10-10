//
// Created by PikachuHy on 2021/4/3.
//

#include "PiDialog.h"
#include <QMessageBox>
PiDialog::PiDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

}

void PiDialog::showInfo(const QString &title, const QString &msg) {

    QMessageBox::information(this, title, msg);
}

void PiDialog::showWarning(const QString &title, const QString &msg) {

    QMessageBox::warning(this, title, msg);
}

void PiDialog::showError(const QString &title, const QString &msg) {

    QMessageBox::critical(this, title, msg);
}

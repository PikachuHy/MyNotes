//
// Created by PikachuHy on 2020/12/27.
//

#include "SearchDialog.h"
#include "Constant.h"
#include <QLineEdit>
#include <QVBoxLayout>
SearchDialog::SearchDialog(QWidget *parent): QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setMinimumSize(Constant::searchDialogWidth, Constant::searchDialogHeight);
    auto vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(m_lineEdit);
    setLayout(vbox);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchDialog::on_lineEdit_textChanged);
}

void SearchDialog::on_lineEdit_textChanged(const QString &text) {
    emit searchTextChanged(text);
}


void SearchDialog::hideEvent(QHideEvent *event) {
    m_lineEdit->clear();
    QWidget::hideEvent(event);
}

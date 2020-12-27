//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_SEARCHDIALOG_H
#define MYNOTES_SEARCHDIALOG_H

#include <QDialog>
class QLineEdit;
class SearchDialog: public QDialog {
Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = nullptr);

protected:

    void hideEvent(QHideEvent *event) override;

signals:
    void searchTextChanged(const QString& text);
public slots:
    void on_lineEdit_textChanged(const QString &text);
private:
    QLineEdit* m_lineEdit;
};


#endif //MYNOTES_SEARCHDIALOG_H

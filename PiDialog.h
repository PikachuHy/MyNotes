//
// Created by PikachuHy on 2021/4/3.
//

#ifndef MYNOTES_PIDIALOG_H
#define MYNOTES_PIDIALOG_H

#include <QDialog>
class PiDialog : public QDialog{
Q_OBJECT
public:
    explicit PiDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void showInfo(const QString& title, const QString& msg);
    void showWarning(const QString& title, const QString& msg);
    void showError(const QString& title, const QString& msg);
private:
};


#endif //MYNOTES_PIDIALOG_H

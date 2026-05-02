//
// Created by PikachuHy on 2021/4/3.
//

#ifndef MYNOTES_LOGINDIALOG_H
#define MYNOTES_LOGINDIALOG_H

#include <QDialog>
#include "PiDialog.h"
#include <QLineEdit>
#include <QCheckBox>
class LoginDialog: public PiDialog {
Q_OBJECT
public:
    LoginDialog();
    void login(const QString& account, const QString& password);
    void updateSettings();
private:
    QLineEdit* m_accountLineEdit;
    QLineEdit* m_passwordLineEdit;
    QCheckBox* m_rememberPasswordCheckBox;
    QCheckBox* m_autoLoginCheckBox;
};


#endif //MYNOTES_LOGINDIALOG_H

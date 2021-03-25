//
// Created by PikachuHy on 2021/3/25.
//

#ifndef MYNOTES_SETTINGSDIALOG_H
#define MYNOTES_SETTINGSDIALOG_H
#include <QSettings>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include "Settings.h"
class SettingsDialog: public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private:
    QLineEdit *m_baseUrlLineEdit;
    QLineEdit *m_ownerLineEdit;
    QLineEdit *m_passwordLineEdit;
    QSpinBox *m_portSpinBox;
    QLineEdit *m_typoraPathLineEdit;
    QPushButton *m_typoraPathChooseBtn;
    Settings *m_settings;
    QPushButton *m_confirmBtn;
    QPushButton *m_cancelBtn;
};


#endif //MYNOTES_SETTINGSDIALOG_H

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
#include <QCheckBox>
#include "Settings.h"
#include "PiDialog.h"
class SettingsDialog: public PiDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private:
    QCheckBox *m_autoSyncWorkshopCheckBox;
    QCheckBox *m_autoSyncWatchingCheckBox;
    QLineEdit *m_serverLineEdit;
    QLineEdit *m_typoraPathLineEdit;
    QPushButton *m_typoraPathChooseBtn;
};


#endif //MYNOTES_SETTINGSDIALOG_H

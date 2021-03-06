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
#include <QRadioButton>
#include "Settings.h"
#include "PiDialog.h"
class SettingsDialog: public PiDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
signals:
    void requestReindex();
private:
    QCheckBox *m_autoSyncWorkshopCheckBox;
    QCheckBox *m_autoSyncWatchingCheckBox;
    QCheckBox *m_enableHiDPICheckBox;
    QLineEdit *m_serverLineEdit;
    QLineEdit *m_typoraPathLineEdit;
    QPushButton *m_typoraPathChooseBtn;
    QLineEdit *m_trojanConfigPathLineEdit;
    QPushButton *m_trojanConfigPathChooseBtn;
    QRadioButton *m_webEngineRenderModeRadioBtn;
    QRadioButton *m_textBrowserRenderModeRadioBtn;
    QRadioButton *m_selfRenderModeRadioBtn;
    bool m_oldHiDPIFlag;
};


#endif //MYNOTES_SETTINGSDIALOG_H

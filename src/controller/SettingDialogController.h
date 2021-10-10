//
// Created by pikachu on 2021/6/15.
//

#ifndef MYNOTES_SETTINGDIALOGCONTROLLER_H
#define MYNOTES_SETTINGDIALOGCONTROLLER_H

#include <QObject>
class SettingDialogController : public QObject{
Q_OBJECT
    Q_PROPERTY(int render READ render WRITE setRender)
public:
    int render();
    void setRender(int value);
private:
};


#endif //MYNOTES_SETTINGDIALOGCONTROLLER_H

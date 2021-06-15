//
// Created by pikachu on 2021/6/15.
//

#include "SettingDialogController.h"
#include "Settings.h"
int SettingDialogController::render() {
    return Settings::instance()->modeRender;
}

void SettingDialogController::setRender(int value) {
    Settings::instance()->modeRender = value;
}


#pragma once

#include <lvgl.h>
#include "iapp_window.h"
#include "../../driver/axp/axp20x.h"

class BatteryWindow : public IAppWindow {

DECLARE_SINGLETONE(BatteryWindow)

public:
    bool Init(AXP20X_Class *power);

private:
    void CreateSubWidgets();

    void ShowCallback();

private:
    AXP20X_Class *power_;
    lv_obj_t *lv_label_;
};

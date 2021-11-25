
#include "battery_window.h"

DEFINIE_SINGLETONE(BatteryWindow)

BatteryWindow::BatteryWindow() : IAppWindow("B") {
}

bool BatteryWindow::Init(AXP20X_Class *power) {
    power_ = power;
    Create();
    return true;
}

void BatteryWindow::CreateSubWidgets() {
    lv_label_ = lv_label_create(container_);
    lv_obj_center(lv_label_);
}

void BatteryWindow::ShowCallback() {
    int per = power_->getBattPercentage();
    lv_label_set_text_fmt(lv_label_, "%d", per);
}



#pragma once

#include <lvgl.h>
#include "iapp_window.h"
#include "../../driver/tft/bl.h"

class BrightnessWindow : public IAppWindow {

DECLARE_SINGLETONE(BrightnessWindow)

public:
    bool Init(BackLight *backlight);

private:
    static void BacklightChangedEventHandler(lv_event_t *event);

private:
    void CreateSubWidgets();

private:
    lv_obj_t *arc_, *label_;
    BackLight *backlight_;
};

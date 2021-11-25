
#pragma once

#include <lvgl.h>
#include "iapp_window.h"

class DialSettingWindow : public IAppWindow {

DECLARE_SINGLETONE(DialSettingWindow)

public:
    bool Init();

private:
    void CreateSubWidgets();

private:
    lv_obj_t *roller_;
    lv_obj_t *btn[3];
};


#pragma once

#include "itime_widget.h"
#include <vector>
#include <lvgl.h>
#include "../../driver/rtc/pcf8563.h"

struct NumberTimeConfig {
    lv_color_t colors[3];
    const lv_font_t *fonts[3];
};

class NumberTimeWidget : public ITimeWidget {
public:
    NumberTimeWidget(PCF8563_Class *rtc, const NumberTimeConfig &time_configs);

    bool Init() override;
    void SetPosition(int16_t x, int16_t y) override;

private:
    void RefreshTime() override;

private:
    void AlignHourSecToMin();

private:
    NumberTimeConfig time_config_;
    int16_t pos_x_, pos_y_;
    lv_obj_t *lv_times_[3];
};


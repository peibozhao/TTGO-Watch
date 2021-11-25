
#pragma once

#include "itime_widget.h"
#include <vector>
#include <lvgl.h>

struct CircleTimeConfig {
    lv_color_t colors[3];
    lv_coord_t radius[3];
    lv_coord_t width[3];
};

class CircleTimeWidget : public ITimeWidget {
public:
    CircleTimeWidget(PCF8563_Class *rtc,
                    const CircleTimeConfig &time_config);

    bool Init() override;
    void SetPosition(int16_t x, int16_t y) override;

private:
    static void GestureEventHandler(lv_event_t *event);

private:
    void RefreshTime() override;

private:
    CircleTimeConfig time_config_;
    int16_t pos_x_, pos_y_;
    lv_obj_t *lv_hor_line_, *lv_ver_line_;
    lv_obj_t *lv_times_[3];
};

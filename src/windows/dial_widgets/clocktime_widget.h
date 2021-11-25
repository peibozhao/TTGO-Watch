
#pragma once

#include "itime_widget.h"
#include <vector>
#include <lvgl.h>
#include "../../driver/rtc/pcf8563.h"

struct ClockTimeConfig {
    lv_color_t colors[3];
    lv_coord_t length[3];
    lv_coord_t line_width[3];
};

class ClockTimeWidget : public ITimeWidget {
public:
    ClockTimeWidget(PCF8563_Class *rtc,
                    const ClockTimeConfig &time_config);

    bool Init() override;
    void SetPosition(int16_t x, int16_t y) override;

private:
    void RefreshTime() override;

    std::vector<lv_point_t> CalcEndPoints();

private:
    ClockTimeConfig time_config_;
    int16_t pos_x_, pos_y_;
    lv_obj_t *lv_led_;
    lv_obj_t *lv_times_[3];
};

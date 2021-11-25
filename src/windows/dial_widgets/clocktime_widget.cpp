
#include "clocktime_widget.h"
#include <math.h>

ClockTimeWidget::ClockTimeWidget(PCF8563_Class *rtc,
                                 const ClockTimeConfig &time_config)
    : ITimeWidget(rtc) {
    time_config_ = time_config;
    pos_x_ = 0, pos_y_ = 0;
}

bool ClockTimeWidget::Init() {
    for (int i = 0; i < 3; ++i) {
        lv_times_[i] = lv_line_create(lv_scr_act());
        lv_objs_.push_back(lv_times_[i]);
        lv_obj_add_flag(lv_times_[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_line_rounded(lv_times_[i], true, 0);
        lv_obj_set_style_line_color(lv_times_[i], time_config_.colors[i], 0);
        lv_obj_set_style_line_width(lv_times_[i], time_config_.line_width[i],
                                    0);
    }
    lv_led_ = lv_led_create(lv_scr_act());
    lv_objs_.push_back(lv_led_);
    lv_obj_add_flag(lv_led_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(lv_led_);
    lv_obj_set_size(lv_led_, 10, 10);
    lv_led_set_color(lv_led_, lv_color_white());
    lv_led_set_brightness(lv_led_, 0);
    SetPosition(0, 0);
    RefreshTime();
}

void ClockTimeWidget::SetPosition(int16_t x, int16_t y) {
    lv_obj_t *parent = lv_obj_get_parent(lv_times_[0]);
    pos_x_ = lv_obj_get_width(parent) / 2 + x;
    pos_y_ = lv_obj_get_height(parent) / 2 + y;
    lv_obj_set_pos(lv_led_, x, y);
    RefreshTime();
}

void ClockTimeWidget::RefreshTime() {
    std::vector<lv_point_t> end_points = CalcEndPoints();
    static lv_point_t points[6];
    for (int i = 0; i < 3; ++i) {
        points[2 * i] = {pos_x_, pos_y_};
        points[2 * i + 1] = end_points[i];
        lv_line_set_points(lv_times_[i], points + 2 * i, 2);
    }
}

std::vector<lv_point_t> ClockTimeWidget::CalcEndPoints() {
    std::vector<lv_point_t> ret(3);
    RTC_Date date = rtc_->getDateTime();

    float range[3] = {12, 60, 60};
    float value[3] = {float(date.hour % 12), float(date.minute),
                      float(date.second)};
    for (int i = 0; i < 3; ++i) {
        int len = time_config_.length[i];
        float percent = value[i] / range[i];
        ret[i].x = pos_x_ + len * sin(2 * PI * percent);
        ret[i].y = pos_y_ - len * cos(2 * PI * percent);
    }
    return ret;
}

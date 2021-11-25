
#include "circletime_widget.h"
#include <math.h>

CircleTimeWidget::CircleTimeWidget(PCF8563_Class *rtc,
                                   const CircleTimeConfig &time_config)
    : ITimeWidget(rtc) {
    time_config_ = time_config;
    pos_x_ = 0, pos_y_ = 0;
}

bool CircleTimeWidget::Init() {
    for (int i = 0; i < 3; ++i) {
        lv_times_[i] = lv_arc_create(lv_scr_act());
        lv_objs_.push_back(lv_times_[i]);
        lv_arc_set_rotation(lv_times_[i], 270);
        lv_arc_set_bg_angles(lv_times_[i], 0, 360);
        lv_obj_add_flag(lv_times_[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_arc_rounded(lv_times_[i], 10, LV_PART_INDICATOR);
        lv_obj_center(lv_times_[i]);
        lv_obj_remove_style(lv_times_[i], nullptr, LV_PART_KNOB);
        lv_obj_clear_flag(lv_times_[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(lv_times_[i], time_config_.radius[i] * 2,
                        time_config_.radius[i] * 2);
        lv_obj_set_style_arc_width(lv_times_[i], time_config_.width[i],
                                   LV_PART_MAIN);
        lv_obj_set_style_arc_width(lv_times_[i], time_config_.width[i],
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(lv_times_[i], time_config_.colors[i],
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_opa(lv_times_[i], LV_OPA_TRANSP, LV_PART_MAIN);
    }
    lv_arc_set_range(lv_times_[0], 0, 12);
    lv_arc_set_range(lv_times_[1], 0, 60);
    lv_arc_set_range(lv_times_[2], 0, 60);

    // Draw line
    lv_hor_line_ = lv_line_create(lv_scr_act());
    lv_ver_line_ = lv_line_create(lv_scr_act());
    lv_objs_.push_back(lv_hor_line_);
    lv_objs_.push_back(lv_ver_line_);
    lv_obj_t *lines[2] = {lv_hor_line_, lv_ver_line_};
    for (int i = 0; i < 2; ++i) {
        lv_obj_add_flag(lines[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_center(lines[i]);
        lv_obj_set_style_line_color(lines[i], lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_line_opa(lines[i], LV_OPA_20, LV_PART_MAIN);
    }
    int16_t width = lv_obj_get_width(lv_scr_act()),
            height = lv_obj_get_height(lv_scr_act());
    lv_coord_t len = width > height ? width / 2 : height / 2 ;
    static lv_point_t hor_points[2] = {{0, 0}, {len, 0}};
    lv_line_set_points(lv_hor_line_, hor_points, 2);
    static lv_point_t ver_points[2] = {{0, 0}, {0, len}};
    lv_line_set_points(lv_ver_line_, ver_points, 2);

    RefreshTime();
}

void CircleTimeWidget::SetPosition(int16_t x, int16_t y) {
    pos_x_ = x;
    pos_y_ = y;
    lv_obj_set_pos(lv_hor_line_, pos_x_, pos_y_);
    lv_obj_set_pos(lv_ver_line_, pos_x_, pos_y_);
    RefreshTime();
}

void CircleTimeWidget::RefreshTime() {
    RTC_Date date = rtc_->getDateTime();
    lv_arc_set_value(lv_times_[0], date.hour % 12);
    lv_arc_set_value(lv_times_[1], date.minute);
    lv_arc_set_value(lv_times_[2], date.second);
}


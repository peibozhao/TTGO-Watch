
#include "numbertime_widget.h"

NumberTimeWidget::NumberTimeWidget(PCF8563_Class *rtc,
                                   const NumberTimeConfig &time_config)
    : ITimeWidget(rtc) {
    time_config_ = time_config;
    pos_x_ = 0, pos_y_ = 0;
}

bool NumberTimeWidget::Init() {
    for (int i = 0; i < 3; ++i) {
        lv_times_[i] = lv_label_create(lv_scr_act());
        lv_objs_.push_back(lv_times_[i]);
        lv_obj_add_flag(lv_times_[i], LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(lv_times_[i], "00");
    }
    lv_obj_center(lv_times_[1]);

    for (int i = 0; i < 3; ++i) {
        lv_obj_set_style_text_font(lv_times_[i], time_config_.fonts[i], 0);
        lv_obj_set_style_text_color(lv_times_[i], time_config_.colors[i], 0);
    }
    AlignHourSecToMin();
}

void NumberTimeWidget::SetPosition(int16_t x, int16_t y) {
    pos_x_ = x;
    pos_y_ = y;
    lv_obj_align(lv_times_[1], LV_ALIGN_CENTER, pos_x_, pos_y_);
    AlignHourSecToMin();
}

void NumberTimeWidget::RefreshTime() {
    RTC_Date date = rtc_->getDateTime();
    lv_label_set_text_fmt(lv_times_[0], "%02u", date.hour);
    lv_label_set_text_fmt(lv_times_[1], "%02u", date.minute);
    lv_label_set_text_fmt(lv_times_[2], "%02u", date.second);

    AlignHourSecToMin();
}

void NumberTimeWidget::AlignHourSecToMin() {
    char *text[3];
    lv_coord_t letter_spaces[3];
    lv_coord_t text_width[3];

    for (int i = 0; i < 3; ++i) {
        text[i] = lv_label_get_text(lv_times_[i]);
        letter_spaces[i] =
            lv_obj_get_style_text_letter_space(lv_times_[i], LV_PART_MAIN);
        text_width[i] = lv_txt_get_width(text[i], 2, time_config_.fonts[i],
                                         letter_spaces[i], LV_TEXT_FLAG_NONE);
    }

    int interval = 10;
    lv_obj_align_to(lv_times_[0], lv_times_[1], LV_ALIGN_CENTER,
                    -(text_width[1] / 2 + interval + text_width[0] / 2), 0);
    lv_obj_align_to(lv_times_[2], lv_times_[1], LV_ALIGN_CENTER,
                    (text_width[1] / 2 + interval + text_width[2] / 2), 0);
}

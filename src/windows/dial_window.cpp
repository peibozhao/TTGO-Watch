
#include "dial_window.h"
#include "menu_window.h"

DialWindow *DialWindow::instance_ = nullptr;

DialWindow *DialWindow::Instance() {
    if (instance_ == nullptr) {
        instance_ = new DialWindow();
    }
    return instance_;
}

bool DialWindow::Init(PCF8563_Class *rtc, int16_t width, int16_t height,
                      const std::vector<BackgroundConfig> &bg_configs,
                      const NumberTimeConfig &number_config,
                      const ClockTimeConfig &clock_config,
                      const CircleTimeConfig &circle_config) {
    width_ = width;
    height_ = height;
    bg_configs_ = bg_configs;
    cur_bg_idx_ = 0;
    time_widgets_[TimeType::Number] = new NumberTimeWidget(rtc, number_config);
    time_widgets_[TimeType::Clock] = new ClockTimeWidget(rtc, clock_config);
    time_widgets_[TimeType::Circle] = new CircleTimeWidget(rtc, circle_config);

    CreateBackground();
    bool ret = true;
    for (auto iter : time_widgets_) {
        ret &= iter.second->Init();
    }
    cur_timetype_ = TimeType::Circle;
    CurrentTimeWidget()->Active();
    return ret;
}

void DialWindow::Hidden() {
    lv_obj_remove_event_cb(lv_scr_act(), &DialWindow::GestureEventHandler);
    CurrentTimeWidget()->Hidden();
    lv_obj_add_flag(lv_background_, LV_OBJ_FLAG_HIDDEN);
}

void DialWindow::Show() {
    lv_obj_clear_flag(lv_background_, LV_OBJ_FLAG_HIDDEN);
    CurrentTimeWidget()->Show();
    lv_obj_add_event_cb(lv_scr_act(), &DialWindow::GestureEventHandler,
                        LV_EVENT_GESTURE, nullptr);
}

void DialWindow::SetTimePosition(int16_t x, int16_t y) {
    CurrentTimeWidget()->SetPosition(x, y);
}

void DialWindow::SetTimeType(TimeType type) {
    CurrentTimeWidget()->Hidden();
    CurrentTimeWidget()->Unactive();
    cur_timetype_ = type;
    CurrentTimeWidget()->Active();
    CurrentTimeWidget()->Show();
}

TimeType DialWindow::GetTimeType() { return cur_timetype_; }

void DialWindow::GestureEventHandler(lv_event_t *event) {
    DialWindow *this_ = DialWindow::Instance();
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    if ((dir & LV_DIR_VER) != 0) {
        // Change background
        int background_count = this_->bg_configs_.size();
        int bg_idx = this_->cur_bg_idx_;
        if ((dir & LV_DIR_TOP) != 0) {
            bg_idx = this_->cur_bg_idx_ == 0 ? background_count - 1
                                             : this_->cur_bg_idx_ - 1;
        } else if ((dir & LV_DIR_BOTTOM) != 0) {
            bg_idx = this_->cur_bg_idx_ == background_count - 1
                         ? 0
                         : this_->cur_bg_idx_ + 1;
        }
        this_->ChangeBackground(bg_idx);
        this_->CurrentTimeWidget()->Hidden();
    } else if ((dir & LV_DIR_LEFT) != 0) {
        MenuWindow::Instance()->Show();
        this_->Hidden();
    } else if ((dir & LV_DIR_RIGHT) != 0) {
        this_->CurrentTimeWidget()->Show();
    }
}

DialWindow::DialWindow() { lv_background_ = nullptr; }

void DialWindow::CreateBackground() {
    lv_background_ = lv_img_create(lv_scr_act());
    lv_obj_add_flag(lv_background_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(lv_background_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(lv_background_, width_, height_);

    ChangeBackground(cur_bg_idx_);
}

void DialWindow::ChangeBackground(uint8_t idx) {
    cur_bg_idx_ = idx;
    BackgroundConfig &bg_config = bg_configs_[idx];
    lv_img_set_src(lv_background_, bg_config.image);
}

ITimeWidget *DialWindow::CurrentTimeWidget() {
    return time_widgets_[cur_timetype_];
}

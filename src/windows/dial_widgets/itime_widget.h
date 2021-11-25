
#pragma once

#include "../../driver/rtc/pcf8563.h"
#include <lvgl.h>
#include <vector>

class ITimeWidget {
public:
    ITimeWidget(PCF8563_Class *rtc) {
        rtc_ = rtc;
        lv_timer_ = nullptr;
    }

    virtual bool Init() = 0;

    virtual void SetPosition(int16_t x, int16_t y) = 0;

    void Show() {
        for (auto ele : lv_objs_) {
            lv_obj_clear_flag(ele, LV_OBJ_FLAG_HIDDEN);
        }
        ShowCallback();
    }

    void Hidden() {
        for (auto ele : lv_objs_) {
            lv_obj_add_flag(ele, LV_OBJ_FLAG_HIDDEN);
        }
        HiddenCallback();
    }

    virtual void Active() {
        if (lv_timer_ == nullptr) {
            lv_timer_ =
                lv_timer_create(&ITimeWidget::TimerTaskHandler, 1000, this);
        }
    }

    virtual void Unactive() {
        if (lv_timer_ != nullptr) {
            lv_timer_del(lv_timer_);
            lv_timer_ = nullptr;
        }
    }

protected:
    static void TimerTaskHandler(lv_timer_t *timer) {
        // ITimeWidget *this_ = (ITimeWidget *)timer->user_data;
        ITimeWidget *this_ = static_cast<ITimeWidget *>(timer->user_data);
        this_->RefreshTime();
    }

protected:
    virtual void RefreshTime() = 0;

    virtual void ShowCallback() {}

    virtual void HiddenCallback() {}

protected:
    PCF8563_Class *rtc_;
    lv_timer_t *lv_timer_;
    std::vector<lv_obj_t *> lv_objs_;
};

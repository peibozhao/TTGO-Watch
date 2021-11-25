
#include "brightness_window.h"

DEFINIE_SINGLETONE(BrightnessWindow)

BrightnessWindow::BrightnessWindow() : IAppWindow("L") {}

bool BrightnessWindow::Init(BackLight *backlight) {
    backlight_ = backlight;
    Create();
}

void BrightnessWindow::CreateSubWidgets() {
    int bl_level = backlight_->getLevel();
    label_ = lv_label_create(container_);
    lv_obj_center(label_);
    lv_label_set_text_fmt(label_, "%d", bl_level);

    arc_ = lv_arc_create(container_);
    lv_obj_center(arc_);
    lv_arc_set_rotation(arc_, 270);
    lv_arc_set_bg_angles(arc_, 0, 360);
    lv_arc_set_range(arc_, 1, 100);

    int16_t len = width_ < height_ ? width_ : height_;
    len -= 60;
    lv_obj_set_size(arc_, len, len);
    lv_obj_set_style_arc_width(arc_, len / 6, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, len / 6, LV_PART_INDICATOR);
    lv_obj_add_event_cb(arc_, &BrightnessWindow::BacklightChangedEventHandler,
                        LV_EVENT_VALUE_CHANGED, this);
    lv_arc_set_value(arc_, bl_level);
}

void BrightnessWindow::BacklightChangedEventHandler(lv_event_t *event) {
    BrightnessWindow *this_ = BrightnessWindow::Instance();
    int16_t value = lv_arc_get_value(this_->arc_);
    lv_label_set_text_fmt(this_->label_, "%d", value);
    this_->backlight_->adjust(value);
}

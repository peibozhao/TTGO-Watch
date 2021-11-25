
#pragma once

#include "../menu_window.h"
#include <lvgl.h>
#include <string>

#define DECLARE_SINGLETONE(class_name)                                         \
public:                                                                        \
    static class_name *Instance();                                             \
                                                                               \
private:                                                                       \
    class_name();                                                              \
    class_name(const class_name &) = delete;                                   \
    class_name &operator=(const class_name &) = delete;                        \
                                                                               \
private:                                                                       \
    static class_name *instance_;

#define DEFINIE_SINGLETONE(class_name)                                         \
    class_name *class_name::instance_ = nullptr;                               \
    class_name *class_name::Instance() {                                       \
        if (instance_ == nullptr) {                                            \
            instance_ = new class_name();                                      \
        }                                                                      \
        return instance_;                                                      \
    }

class IAppWindow {
public:
    IAppWindow(const std::string &name) {
        name_ = name;
        container_ = nullptr;
    }

    virtual void Hidden() {
        HiddenCallback();
        lv_obj_remove_event_cb(lv_scr_act(), &IAppWindow::GestureEventHandler);
        lv_obj_add_flag(container_, LV_OBJ_FLAG_HIDDEN);
    }

    virtual void Show() {
        ShowCallback();
        lv_obj_clear_flag(container_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(lv_scr_act(), &IAppWindow::GestureEventHandler,
                            LV_EVENT_GESTURE, this);
    }

protected:
    static void GestureEventHandler(lv_event_t *event) {
        IAppWindow *this_ = (IAppWindow *)event->user_data;
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_RIGHT) {
            this_->Hidden();
            MenuWindow::Instance()->Show();
        }
    }

protected:
    virtual void CreateSubWidgets() = 0;

    // Need called in init
    virtual void Create() {
        container_ = lv_obj_create(lv_scr_act());
        lv_obj_add_flag(container_, LV_OBJ_FLAG_HIDDEN);
        width_ = lv_obj_get_width(lv_scr_act());
        height_ = lv_obj_get_height(lv_scr_act());
        lv_obj_set_size(container_, width_, height_);
        CreateSubWidgets();
        Register();
    }

    // Related to menu window
    virtual void Register() {
        ApplicationConfig app_config;
        app_config.name = name_;
        app_config.click_handler = [](lv_event_t *event) {
            lv_point_t point;
            lv_indev_get_point(lv_indev_get_act(), &point);
            lv_obj_t *obj = lv_event_get_target(event);
            int16_t obj_x_min = lv_obj_get_x(obj),
                    obj_x_max = lv_obj_get_x(obj) + lv_obj_get_width(obj);
            int16_t obj_y_min = lv_obj_get_y(obj),
                    obj_y_max = lv_obj_get_y(obj) + lv_obj_get_height(obj);

            if (point.x < obj_x_min || point.x > obj_x_max ||
                point.y < obj_y_min || point.y > obj_y_max) {
                return;
            }

            IAppWindow *this_ = (IAppWindow *)lv_event_get_user_data(event);
            this_->Show();
            MenuWindow::Instance()->Hidden();
        };
        app_config.click_userdata = this;
        MenuWindow::Instance()->Register(app_config);
    }

    virtual void ShowCallback() {}

    virtual void HiddenCallback() {}

protected:
    int16_t width_, height_;
    lv_obj_t *container_;
    std::string name_;
};

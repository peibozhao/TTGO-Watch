
#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <lvgl.h>

struct ApplicationConfig {
    std::string name;
    void (*click_handler)(lv_event_t *);
    void *click_userdata;

    ApplicationConfig() {
        click_handler = nullptr;
        click_userdata = nullptr;
    }
};

struct MenuWindowConfig {
    int16_t width, height;
    const lv_img_dsc_t *background;
    const lv_font_t *font;
};

class MenuWindow {
public:
    static MenuWindow *Instance();

public:
    bool Init(const MenuWindowConfig &win_config);
    void Register(const ApplicationConfig &app_config);

    void Hidden();
    void Show();

private:
    static void GestureEventHandler(lv_event_t *event);

private:
    MenuWindow();
    MenuWindow(const MenuWindow &) = delete;
    MenuWindow & operator= (const MenuWindow &) = delete;

    void CreateWidgets();

private:
    static MenuWindow *instance_;

private:
    MenuWindowConfig config_;
    lv_obj_t *tileview_;
    std::vector<lv_obj_t *> tiles_;

    std::list<ApplicationConfig> app_configs_;
};


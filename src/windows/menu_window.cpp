
#include "menu_window.h"
#include "dial_window.h"

MenuWindow *MenuWindow::instance_ = nullptr;

MenuWindow *MenuWindow::Instance() {
    if (instance_ == nullptr) {
        instance_ = new MenuWindow();
    }
    return instance_;
}

MenuWindow::MenuWindow() { tileview_ = nullptr; }

bool MenuWindow::Init(const MenuWindowConfig &win_config) {
    config_ = win_config;
    CreateWidgets();
    return true;
}

void MenuWindow::Register(const ApplicationConfig &app_config) {
    uint8_t page = app_configs_.size() / 9;
    uint8_t col = app_configs_.size() % 9 % 3;
    uint8_t row = app_configs_.size() % 9 / 3;
    app_configs_.push_back(app_config);

    // Create application button
    lv_coord_t grid_cell_width = config_.width / 3,
               grid_cell_height = config_.height / 3;
    static lv_coord_t col_dsc[] = {grid_cell_width, grid_cell_width,
                                   grid_cell_width, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {grid_cell_height, grid_cell_height,
                                   grid_cell_height, LV_GRID_TEMPLATE_LAST};

    if (page >= tiles_.size()) {
        lv_obj_t *tile = lv_tileview_add_tile(tileview_, page, 0, LV_DIR_HOR);
        lv_obj_set_style_bg_img_src(tile, config_.background, 0);

        lv_obj_set_style_grid_column_dsc_array(tile, col_dsc, 0);
        lv_obj_set_style_grid_row_dsc_array(tile, row_dsc, 0);
        lv_obj_set_size(tile, config_.width, config_.height);
        lv_obj_center(tile);
        lv_obj_set_layout(tile, LV_LAYOUT_GRID);
        tiles_.push_back(tile);
    }

    lv_obj_t *btn = lv_btn_create(tiles_.back());
    lv_obj_set_size(btn, grid_cell_width, grid_cell_height);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_text_font(label, config_.font, LV_PART_MAIN);
    lv_label_set_text(label, app_config.name.c_str());
    lv_obj_center(label);

    if (app_config.click_handler != nullptr) {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, app_config.click_handler, LV_EVENT_SHORT_CLICKED,
                            app_config.click_userdata);
    }
}

void MenuWindow::Hidden() {
    lv_obj_remove_event_cb(lv_scr_act(), &MenuWindow::GestureEventHandler);
    lv_obj_add_flag(tileview_, LV_OBJ_FLAG_HIDDEN);
}

void MenuWindow::Show() {
    lv_obj_clear_flag(tileview_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(lv_scr_act(), &MenuWindow::GestureEventHandler,
                        LV_EVENT_GESTURE, nullptr);
}

void MenuWindow::GestureEventHandler(lv_event_t *event) {
    MenuWindow *this_ = MenuWindow::Instance();
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_RIGHT) {
        this_->Hidden();
        DialWindow::Instance()->Show();
    }
}

void MenuWindow::CreateWidgets() {
    tileview_ = lv_tileview_create(lv_scr_act());
    lv_obj_add_flag(tileview_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(tileview_, config_.width, config_.height);
}

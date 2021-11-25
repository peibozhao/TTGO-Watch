
#include "dialsetting_window.h"
#include "../dial_window.h"
#include <map>
#include <string>

LV_FONT_DECLARE(lv_font_montserrat_40)

static std::map<TimeType, std::string> type_to_names = {
    {TimeType::Number, "Num"},
    {TimeType::Clock, "Clock"},
    {TimeType::Circle, "Circle"}};

DEFINIE_SINGLETONE(DialSettingWindow)

DialSettingWindow::DialSettingWindow() : IAppWindow("D") {}

bool DialSettingWindow::Init() {
    Create();
    return true;
}

void DialSettingWindow::CreateSubWidgets() {
    lv_obj_set_style_bg_color(container_, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(container_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(container_, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(container_, 0, LV_PART_MAIN);

    int row_count = 2, col_count = 2;
    lv_coord_t grid_cell_width = width_ / row_count,
               grid_cell_height = height_ / col_count;
    static lv_coord_t col_dsc[] = {grid_cell_width, grid_cell_width,
                                   LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {grid_cell_height, grid_cell_height,
                                   LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container_, col_dsc, LV_PART_MAIN);
    lv_obj_set_style_grid_row_dsc_array(container_, row_dsc, LV_PART_MAIN);
    lv_obj_set_layout(container_, LV_LAYOUT_GRID);

    for (int timetype = TimeType::Number; timetype <= TimeType::Circle;
         ++timetype) {
        int col_idx = timetype % row_count, row_idx = timetype / col_count;
        lv_obj_t *btn = lv_btn_create(container_);
        lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_size(btn, width_ / row_count - 20, height_ / col_count - 20);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, col_idx, 1,
                             LV_GRID_ALIGN_CENTER, row_idx, 1);

        lv_obj_t *label = lv_label_create(btn);
        lv_obj_center(label);
        lv_label_set_text(label, type_to_names[(TimeType)timetype].c_str());

        lv_obj_add_event_cb(btn,
                            [](lv_event_t *e) {
                                TimeType type = TimeType(int(e->user_data));
                                DialWindow::Instance()->SetTimeType(type);

                                DialWindow::Instance()->Show();
                                DialSettingWindow::Instance()->Hidden();
                            },
                            LV_EVENT_SHORT_CLICKED, (void *)timetype);
    }
}

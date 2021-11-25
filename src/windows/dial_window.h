
#pragma once

#include <lvgl.h>
#include <map>
#include <vector>
#include "dial_widgets/numbertime_widget.h"
#include "dial_widgets/clocktime_widget.h"
#include "dial_widgets/circletime_widget.h"

struct BackgroundConfig {
    const lv_img_dsc_t *image;
};

enum TimeType { Number, Clock, Circle };

class DialWindow {
public:
    static DialWindow *Instance();

public:
    bool Init(PCF8563_Class *rtc, int16_t width, int16_t height,
              const std::vector<BackgroundConfig> &bg_configs,
              const NumberTimeConfig &number_config,
              const ClockTimeConfig &clock_config,
              const CircleTimeConfig &circle_config);

    void Show();
    void Hidden();

    // Relative coordination to screen center
    void SetTimePosition(int16_t x, int16_t y);

    void SetTimeType(TimeType type);

    TimeType GetTimeType();

private:
    static void GestureEventHandler(lv_event_t *event);

private:
    DialWindow();
    DialWindow(const DialWindow &) = delete;
    DialWindow &operator=(const DialWindow &) = delete;

    void CreateBackground();

    void ChangeBackground(uint8_t idx);

    ITimeWidget *CurrentTimeWidget();

private:
    static DialWindow *instance_;

private:
    int16_t width_, height_;
    lv_obj_t *lv_background_;
    std::vector<BackgroundConfig> bg_configs_;
    uint8_t cur_bg_idx_;

    std::map<TimeType, ITimeWidget *> time_widgets_;
    TimeType cur_timetype_;
};

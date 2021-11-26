
#pragma once

#include "../driver/axp/axp20x.h"
#include "../driver/fx50xx/focaltech.h"
#include "../driver/i2c/i2c_bus.h"
#include "../driver/rtc/pcf8563.h"
#include "../driver/tft/bl.h"
#include "TFT_eSPI.h"
#include "ifirmware.h"
#include "lvgl.h"

enum class InterruptType { None, PowerButton, Touchboard };

class LilyGoTWatch2020V3 : public IFirmware {
private:
    enum class Status { Sleep, Active };

public:
    static LilyGoTWatch2020V3 *Instance();

public:
    void Setup() override;

    void Loop() override;

public:
    void SetWakupBacklightLevel(int level);

private:
    static uint8_t I2CReadBytes(uint8_t devAddress, uint8_t regAddress,
                                uint8_t *data, uint8_t len);

    static uint8_t I2CWriteBytes(uint8_t devAddress, uint8_t regAddress,
                                 uint8_t *data, uint8_t len);

    static void DisplayFlush(lv_disp_drv_t *disp, const lv_area_t *area,
                             lv_color_t *color_p);

    static void TouchpadHandler(lv_indev_drv_t *indev_driver,
                                lv_indev_data_t *data);

private:
    LilyGoTWatch2020V3();
    LilyGoTWatch2020V3(const LilyGoTWatch2020V3 &) = delete;
    LilyGoTWatch2020V3 &operator=(const LilyGoTWatch2020V3 &) = delete;

    bool InitPower();
    bool InitBackLight();
    bool InitDisplay();
    bool InitTouchboard();

    // Save power
    void Sleep();
    // Interactivity
    void Active();

    void ProcessInterrupt();
    // Power button short press interrupt
    void PowerInterrupt();
    // Touch board intterrupt
    void TouchboardInterrupt();

    void CreateDisplay();

private:
    static LilyGoTWatch2020V3 *instance_;

private:
    I2CBus *i2c_;
    AXP20X_Class *power_;
    BackLight *backlight_;
    TFT_eSPI *tft_;
    FocalTech_Class *touchboard_;
    PCF8563_Class *rtc_;

    uint8_t backlight_level_;

    // LVGL
    const uint16_t screen_width_ = 240;
    const uint16_t screen_height_ = 240;
    lv_disp_draw_buf_t draw_buf_;
    lv_color_t *buf1_, *buf2_;

    // Interrupt
    Status status_;
    InterruptType interrupt_;
    unsigned long last_active_time_;
    const unsigned long goto_sleep_time_ = 10 * 1000;
};

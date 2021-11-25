
#include "watch_2020_v3.h"
#include "../windows/app_windows/battery_window.h"
#include "../windows/app_windows/brightness_window.h"
#include "../windows/app_windows/dialsetting_window.h"
#include "../windows/app_windows/test_window.h"
#include "../windows/dial_window.h"
#include "../windows/menu_window.h"
#include <vector>

LV_FONT_DECLARE(lv_font_montserrat_20);
LV_FONT_DECLARE(lv_font_montserrat_30);
LV_FONT_DECLARE(lv_font_montserrat_40);
LV_FONT_DECLARE(lv_font_montserrat_48);
LV_IMG_DECLARE(black_240x240);
LV_IMG_DECLARE(dial_240x240);

// UART/I2C configuration
#define BACKLIGHT_PORT 15
#define TOUCHBOARD_REST_PORT 14
#define TOUCHBOARD_I2C_SDA 23
#define TOUCHBOARD_I2C_SCL 32

uint8_t LilyGoTWatch2020V3::I2CReadBytes(uint8_t devAddress, uint8_t regAddress,
                                         uint8_t *data, uint8_t len) {
    instance_->i2c_->readBytes(devAddress, regAddress, data, len);
    return true;
}

uint8_t LilyGoTWatch2020V3::I2CWriteBytes(uint8_t devAddress,
                                          uint8_t regAddress, uint8_t *data,
                                          uint8_t len) {
    instance_->i2c_->writeBytes(devAddress, regAddress, data, len);
    return true;
}

void LilyGoTWatch2020V3::DisplayFlush(lv_disp_drv_t *disp,
                                      const lv_area_t *area,
                                      lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    instance_->tft_->startWrite();
    instance_->tft_->setAddrWindow(area->x1, area->y1, w, h);
    instance_->tft_->pushColors((uint16_t *)&color_p->full, w * h);
    instance_->tft_->endWrite();

    lv_disp_flush_ready(disp);
}

static int16_t old_x, old_y;

void LilyGoTWatch2020V3::TouchpadHandler(lv_indev_drv_t *indev_driver,
                                         lv_indev_data_t *data) {
    uint16_t x, y;
    bool touched = instance_->touchboard_->getPoint(x, y);
    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
        old_x = x;
        old_y = y;
    } else {
        data->state = LV_INDEV_STATE_REL;
        data->point.x = old_x;
        data->point.y = old_y;
    }
}

LilyGoTWatch2020V3 *LilyGoTWatch2020V3::instance_ = nullptr;

LilyGoTWatch2020V3 *LilyGoTWatch2020V3::Instance() {
    if (instance_ == nullptr) {
        instance_ = new LilyGoTWatch2020V3();
    }
    return instance_;
}

void LilyGoTWatch2020V3::Setup() {
    // Init
    tft_ = new TFT_eSPI(screen_width_, screen_height_);
    i2c_ = new I2CBus();
    rtc_ = new PCF8563_Class(*i2c_);

    bool init_power_ret = InitPower();
    Serial.printf("Init power %d\n", init_power_ret);
    bool init_backlight_ret = InitBackLight();
    Serial.printf("Init backlight %d\n", init_backlight_ret);
    bool init_display_ret = InitDisplay();
    Serial.printf("Init display %d\n", init_display_ret);
    bool init_touchboard_ret = InitTouchboard();
    Serial.printf("Init touchboard %d\n", init_touchboard_ret);

    rtc_->setDateTime(RTC_Date(__DATE__, __TIME__));
    power_->setPowerOutPut(AXP202_LDO2, AXP202_ON);
    backlight_->adjust(6);
    backlight_->on();

    // Display
    CreateDisplay();
}

void LilyGoTWatch2020V3::Loop() {
    long last_time = millis();
    lv_task_handler();
    delay(5);
    lv_tick_inc(int(millis() - last_time));
}

bool LilyGoTWatch2020V3::InitPower() {
    // Copy from TTGO_TWatch_Library
    power_ = new AXP20X_Class();
    int ret = power_->begin(LilyGoTWatch2020V3::I2CReadBytes,
                            LilyGoTWatch2020V3::I2CWriteBytes);
    if (ret == AXP_FAIL) {
        Serial.println("AXP Power begin failed");
        return false;
    } else {
        power_->setShutdownTime(AXP_POWER_OFF_TIME_4S);
        power_->setChgLEDMode(AXP20X_LED_OFF);
        power_->setPowerOutPut(AXP202_EXTEN, false);
        power_->setChargeControlCur(300);
        power_->setLDO2Voltage(3300);
        power_->limitingOff();
        power_->setPowerOutPut(AXP202_LDO4, false);
        power_->setLDO4Voltage(AXP202_LDO4_3300MV);
        power_->setPowerOutPut(AXP202_LDO4, true);
        power_->setPowerOutPut(AXP202_LDO3, false);
        return true;
    }
}

bool LilyGoTWatch2020V3::InitBackLight() {
    backlight_ = new BackLight(BACKLIGHT_PORT);
    backlight_->begin();
    return true;
}

bool LilyGoTWatch2020V3::InitDisplay() {
    lv_init();
    tft_->begin();        /* TFT init */
    tft_->setRotation(2);

    // uint16_t screen_calibration[] = {275, 3620, 264, 3532, 1};
    // tft_->setTouch(screen_calibration);

    buf1_ = new lv_color_t[60 * screen_width_];
    buf2_ = new lv_color_t[60 * screen_width_];
    lv_disp_draw_buf_init(&draw_buf_, buf1_, buf2_, 60 * screen_width_);

    // Initialize the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screen_width_;
    disp_drv.ver_res = screen_height_;
    disp_drv.flush_cb = LilyGoTWatch2020V3::DisplayFlush;
    disp_drv.draw_buf = &draw_buf_;
    if (lv_disp_drv_register(&disp_drv) == nullptr) {
        return false;
    }

    // Initialize the input device
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = LilyGoTWatch2020V3::TouchpadHandler;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    if (indev == nullptr) {
        return false;
    }
    return true;
}

bool LilyGoTWatch2020V3::InitTouchboard() {
    pinMode(TOUCHBOARD_REST_PORT, OUTPUT);
    digitalWrite(TOUCHBOARD_REST_PORT, LOW);
    delay(8);
    digitalWrite(TOUCHBOARD_REST_PORT, HIGH);

    touchboard_ = new FocalTech_Class();
    Wire1.begin(TOUCHBOARD_I2C_SDA, TOUCHBOARD_I2C_SCL);
    if (!touchboard_->begin(Wire1)) {
        Serial.println("Begin touch FAIL");
        return false;
    }
    return true;
}

void LilyGoTWatch2020V3::CreateDisplay() {
    std::vector<BackgroundConfig> bg_configs;
    BackgroundConfig bg_config;
    bg_config.image = &black_240x240;
    bg_configs.push_back(bg_config);
    bg_config.image = &dial_240x240;
    bg_configs.push_back(bg_config);

    NumberTimeConfig num_config;
    num_config.colors[0] = lv_color_white();
    num_config.colors[1] = lv_color_white();
    num_config.colors[2] = lv_color_white();
    num_config.fonts[0] = &lv_font_montserrat_48;
    num_config.fonts[1] = &lv_font_montserrat_48;
    num_config.fonts[2] = &lv_font_montserrat_48;

    ClockTimeConfig clock_config;
    clock_config.colors[0] = lv_color_make(0xff, 0xff, 0xff);
    clock_config.colors[1] = lv_color_make(0xff, 0xff, 0xff);
    clock_config.colors[2] = lv_color_make(0xff, 0, 0);
    clock_config.length[0] = 70;
    clock_config.length[1] = 100;
    clock_config.length[2] = 90;
    clock_config.line_width[0] = 6;
    clock_config.line_width[1] = 4;
    clock_config.line_width[2] = 2;

    CircleTimeConfig circle_config;
    circle_config.colors[0] = lv_color_make(0xff, 0, 0);
    circle_config.colors[1] = lv_color_make(0, 0xff, 0);
    circle_config.colors[2] = lv_color_make(0, 0, 0xff);
    circle_config.radius[0] = 40;
    circle_config.radius[1] = 80;
    circle_config.radius[2] = 120;
    circle_config.width[0] = 41;
    circle_config.width[1] = 41;
    circle_config.width[2] = 41;

    DialWindow::Instance()->Init(rtc_, screen_width_, screen_height_,
                                 bg_configs, num_config, clock_config,
                                 circle_config);

    MenuWindowConfig menuwin_config;
    menuwin_config.width = screen_width_;
    menuwin_config.height = screen_height_;
    menuwin_config.background = &black_240x240;
    menuwin_config.font = &lv_font_montserrat_40;
    MenuWindow::Instance()->Init(menuwin_config);

    // App window initialized
    BrightnessWindow::Instance()->Init(backlight_);
    BatteryWindow::Instance()->Init(power_);
    DialSettingWindow::Instance()->Init();
    TestWindow::Instance()->Init();

    DialWindow::Instance()->Show();
}

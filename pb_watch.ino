
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "src/firmware/watch_2020_v3.h"

void setup() {
    Serial.begin(115200);
    LilyGoTWatch2020V3::Instance()->Setup();
}

void loop() {
    LilyGoTWatch2020V3::Instance()->Loop();
}


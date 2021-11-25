
#pragma once

#include <lvgl.h>
#include "iapp_window.h"

class TestWindow : public IAppWindow {

DECLARE_SINGLETONE(TestWindow)

public:
    bool Init();

private:
    void CreateSubWidgets();
};

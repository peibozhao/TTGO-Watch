
#include "test_window.h"

DEFINIE_SINGLETONE(TestWindow)

TestWindow::TestWindow() : IAppWindow("T") {
}

bool TestWindow::Init() {
    Create();
    return true;
}

void TestWindow::CreateSubWidgets() {
    lv_obj_t *label = lv_label_create(container_);
    lv_label_set_text(label, "Test window");
    lv_obj_center(label);
}


#pragma once
#include <Adafruit_SSD1306.h>

class DashboardDisplay {
public:
    static void begin();
    static void render();
};
#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

class DisplayManager {
public:
    static void init();
};
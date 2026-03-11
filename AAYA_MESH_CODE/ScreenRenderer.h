#pragma once
#include <Arduino.h>
#include "UIController.h"

class ScreenRenderer {
public:
    static void begin();
    static void render();

private:
    static void drawBoot();
    static void drawScreen();
    static void drawMenu();
    static void drawConfirm();
    static void drawTopBar();
};
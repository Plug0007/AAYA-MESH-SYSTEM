#pragma once
#include <Arduino.h>
#include "ButtonHandler.h"

enum UIState {
    UI_BOOT,
    UI_SCREEN,
    UI_MENU,
    UI_CONFIRM
};

class UIController {
public:
    static void begin();
    static void update();
    static UIState getState() { return currentState; }
    static uint8_t getScreenIndex() { return currentScreen; }

private:
    static UIState currentState;
    static uint8_t currentScreen;
    static uint8_t currentMenuIndex;
    static bool confirmSelection;

    static unsigned long bootStartTime;
};
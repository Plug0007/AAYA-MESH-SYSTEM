#pragma once

enum ButtonEvent {
    BUTTON_NONE,
    BUTTON_SHORT_PRESS,
    BUTTON_LONG_PRESS
};

class ButtonHandler {
public:
    static void begin();
    static void update();
    static ButtonEvent getEvent();

private:
    static bool lastState;
    static unsigned long pressStartTime;
    static bool longPressTriggered;
    static ButtonEvent currentEvent;
};
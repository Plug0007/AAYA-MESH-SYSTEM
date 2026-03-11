#include "ButtonHandler.h"
#include <Arduino.h>

#define BUTTON_PIN 0
#define DEBOUNCE_DELAY 50
#define LONG_PRESS_TIME 900

bool ButtonHandler::lastState = HIGH;
unsigned long ButtonHandler::pressStartTime = 0;
bool ButtonHandler::longPressTriggered = false;
ButtonEvent ButtonHandler::currentEvent = BUTTON_NONE;

void ButtonHandler::begin() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void ButtonHandler::update() {

    bool currentState = digitalRead(BUTTON_PIN);

    // Button pressed (LOW because pullup)
    if (lastState == HIGH && currentState == LOW) {
        pressStartTime = millis();
        longPressTriggered = false;
    }

    // Button held
    if (currentState == LOW && !longPressTriggered) {
        if (millis() - pressStartTime > LONG_PRESS_TIME) {
            currentEvent = BUTTON_LONG_PRESS;
            longPressTriggered = true;
        }
    }

    // Button released
    if (lastState == LOW && currentState == HIGH) {
        if (!longPressTriggered &&
            millis() - pressStartTime > DEBOUNCE_DELAY) {
            currentEvent = BUTTON_SHORT_PRESS;
        }
    }

    lastState = currentState;
}

ButtonEvent ButtonHandler::getEvent() {
    ButtonEvent event = currentEvent;
    currentEvent = BUTTON_NONE;
    return event;
}
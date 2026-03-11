#include "UIController.h"
#include "ButtonHandler.h"
#include <Arduino.h>

#define TOTAL_SCREENS 6
#define TOTAL_MENU_OPTIONS 4
#define BOOT_DURATION 3000

UIState UIController::currentState = UI_BOOT;
uint8_t UIController::currentScreen = 0;
uint8_t UIController::currentMenuIndex = 0;
bool UIController::confirmSelection = false;
unsigned long UIController::bootStartTime = 0;

void UIController::begin() {

    currentState = UI_BOOT;
    currentScreen = 0;
    currentMenuIndex = 0;
    confirmSelection = false;

    bootStartTime = millis();
}

void UIController::update() {

    ButtonEvent ev = ButtonHandler::getEvent();

    // ================= BOOT =================
    if (currentState == UI_BOOT) {

        static unsigned long bootStart = millis();

    if (currentState == UI_BOOT && millis() - bootStart > 2000) {
        currentState = UI_SCREEN;
    }
    }

    // ================= MAIN SCREENS =================
    if (currentState == UI_SCREEN) {

        if (ev == BUTTON_SHORT_PRESS) {
            currentScreen++;
            if (currentScreen >= TOTAL_SCREENS) {
                currentScreen = 0;
            }
        }

        if (ev == BUTTON_LONG_PRESS) {
            currentState = UI_MENU;
            currentMenuIndex = 0;
        }

        return;
    }

    // ================= MENU =================
    if (currentState == UI_MENU) {

        if (ev == BUTTON_SHORT_PRESS) {
            currentMenuIndex++;
            if (currentMenuIndex >= TOTAL_MENU_OPTIONS) {
                currentMenuIndex = 0;
            }
        }

        if (ev == BUTTON_LONG_PRESS) {

            // Example: option index 2 is critical
            if (currentMenuIndex == 2) {
                currentState = UI_CONFIRM;
                confirmSelection = false;  // Default to NO
            } else {
                currentState = UI_SCREEN;
            }
        }

        return;
    }

    // ================= CONFIRM =================
    if (currentState == UI_CONFIRM) {

        if (ev == BUTTON_SHORT_PRESS) {
            confirmSelection = !confirmSelection;
        }

        if (ev == BUTTON_LONG_PRESS) {

            if (confirmSelection) {
                Serial.println("Confirmed Action");
                // TODO: trigger real action here
            }

            currentState = UI_SCREEN;
        }

        return;
    }
}
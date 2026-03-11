#include "ScreenRenderer.h"
#include "DisplayManager.h"
#include "RoleController.h"
#include "NodeIdentity.h"
#include <WiFi.h>

extern Adafruit_SSD1306 display;

void ScreenRenderer::begin() {
    display.clearDisplay();
    display.display();
}

void ScreenRenderer::render() {

    display.clearDisplay();

    UIState state = UIController::getState();

    switch (state) {

        case UI_BOOT:
            drawBoot();
            break;

        case UI_SCREEN:
            drawScreen();
            break;

        case UI_MENU:
            drawMenu();
            break;

        case UI_CONFIRM:
            drawConfirm();
            break;
    }

    display.display();
}

// ---------------- BOOT ----------------
void ScreenRenderer::drawBoot() {

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(10, 10);
    display.println("AYA-MESH SYSTEM");

    display.setCursor(10, 25);
    display.println("Created By Team R.P");

    display.setCursor(10, 40);
    display.println("Version 2.0");

}
// ---------------- TOP BAR ----------------
void ScreenRenderer::drawTopBar() {

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("AYA ");

    if (WiFi.status() == WL_CONNECTED)
        display.print("W ");
    else
        display.print("X ");

    display.print("RSSI:");
    display.print(WiFi.RSSI());
}

// ---------------- SCREEN VIEW ----------------
void ScreenRenderer::drawScreen() {

    drawTopBar();

    display.setCursor(0, 15);
    display.setTextSize(1);

    display.print("Mode: ");
    display.println(RoleController::getRole() == ROLE_SINK ? "SINK" : "COLL");

    display.print("Node: ");
    display.println(NodeIdentity::getNodeID());

    display.print("Screen: ");
    display.println(UIController::getScreenIndex());  // Add getter
}

// ---------------- MENU ----------------
void ScreenRenderer::drawMenu() {

    display.setTextSize(1);
    display.setCursor(0, 15);
    display.println("> Option 1");
    display.println("  Option 2");
    display.println("  Option 3");
    display.println("  Back");
}

// ---------------- CONFIRM ----------------
void ScreenRenderer::drawConfirm() {

    display.setTextSize(1);
    display.setCursor(10, 20);
    display.println("Are you sure?");

    display.setCursor(20, 40);
    display.println("> YES   NO");
}
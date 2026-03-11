#include "PacketManager.h"
#include "RoleController.h"
#include "NodeConfig.h"
#include "WiFiGateway.h"
#include "GatewayQueue.h"
#include "MQTTGateway.h"
#include "DisplayManager.h"
#include "ButtonHandler.h"
#include "UIController.h"
#include "ScreenRenderer.h"
#include <SPIFFS.h>
#include "DashboardDisplay.h" 

NodeConfig config;

void setup() {

    Serial.begin(115200);
    Serial.println("=== SETUP START ===");

    // 1️⃣ Mount SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
    } else {
        Serial.println("SPIFFS Mounted");
    }

    // 2️⃣ Load configuration
    config = ConfigManager::load();

    // 3️⃣ Initialize role
    RoleController::begin(config);

    // 4️⃣ Initialize display
    DashboardDisplay::begin();

    // 5️⃣ IMPORTANT: start packet system (starts LoRa)
    PacketManager::begin();

    // 6️⃣ Queue system
    GatewayQueue::begin();
    Serial.println("=== QUEUE INIT DONE ===");

    // 7️⃣ Networking
    WiFiGateway::begin();
    MQTTGateway::begin();

    // 8️⃣ UI system
    ButtonHandler::begin();
    UIController::begin();
    ScreenRenderer::begin();
}

void loop() {

    // ================= UI FIRST (HIGH PRIORITY) =================
    //ButtonHandler::update();      // Read button hardware
    //UIController::update();       // Process state machine
    //ScreenRenderer::render();     // Draw frame
    DashboardDisplay::render();
    // ================= NETWORKING SECOND =================
    PacketManager::update();
    GatewayQueue::process();
    WiFiGateway::maintainConnection();
    MQTTGateway::maintainConnection();
}
#include "WiFiGateway.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "RoleController.h"


const char* ssid = "iQOO Neo 10";
const char* password = "Raelyaan";
//const char* ssid = "GNXS-5-600D66_5G";
//const char* password = "200C86958178";

// Test endpoint
const char* serverUrl = "http://httpbin.org/post";

static unsigned long lastReconnectAttempt = 0;
static bool isConnecting = false;

#define WIFI_RETRY_INTERVAL 5000
#define WIFI_CONNECT_TIMEOUT 15000   // 15 seconds max connect time

// ================= BEGIN =================
void WiFiGateway::begin() {

    if (RoleController::getRole() != ROLE_SINK) {
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(true);

    delay(100);

    Serial.println("WiFi Connecting...");
    WiFi.begin(ssid, password);

    isConnecting = true;
    lastReconnectAttempt = millis();
}

// ================= MAINTAIN CONNECTION =================
void WiFiGateway::maintainConnection() {

    if (RoleController::getRole() != ROLE_SINK) {
        return;
    }

    wl_status_t status = WiFi.status();

    // Already connected
    if (status == WL_CONNECTED) {
        if (isConnecting) {
            Serial.println("WiFi Connected");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            isConnecting = false;
        }
        return;
    }

    // If currently trying to connect, wait until timeout
    if (isConnecting) {
        if (millis() - lastReconnectAttempt > WIFI_CONNECT_TIMEOUT) {
            Serial.println("WiFi Connect Timeout");
            isConnecting = false;
        }
        return;
    }

    // Retry only every interval
    if (millis() - lastReconnectAttempt > WIFI_RETRY_INTERVAL) {

        Serial.println("Reconnecting WiFi...");

        WiFi.disconnect(true);
        delay(100);

        WiFi.begin(ssid, password);

        isConnecting = true;
        lastReconnectAttempt = millis();
    }
}

// ================= SEND DATA =================
bool WiFiGateway::sendSensorData(float temp, float humidity) {

    if (RoleController::getRole() != ROLE_SINK) {
        return false;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"temperature\":" + String(temp) + ",";
    payload += "\"humidity\":" + String(humidity);
    payload += "}";

    int httpResponseCode = http.POST(payload);

    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);

    http.end();

    return (httpResponseCode == 200);
}

// ================= STATUS CHECK =================
bool WiFiGateway::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
#pragma once
#include <Arduino.h>

class WiFiGateway {
public:
    static void begin();
    static void maintainConnection();   // NEW
    static bool sendSensorData(float temp, float humidity);
    static bool isConnected();
};
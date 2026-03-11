#pragma once
#include <Arduino.h>

class MQTTGateway {
public:
    static void begin();
    static void maintainConnection();
    static bool publishSensor(float temp, float humidity);
    static bool isConnected();
};
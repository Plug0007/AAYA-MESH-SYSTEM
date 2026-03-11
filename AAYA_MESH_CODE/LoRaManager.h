#pragma once
#include <Arduino.h>

class LoRaManager {
public:
    static bool begin();
    static bool send(uint8_t* data, size_t length);
    static bool receive(uint8_t* buffer, size_t& length);
};

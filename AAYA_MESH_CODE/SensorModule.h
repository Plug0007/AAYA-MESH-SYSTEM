#pragma once
#include <stdint.h>

class SensorModule {
public:
    static void begin();
    static void buildPayload(uint8_t* buffer, uint8_t& length);
};
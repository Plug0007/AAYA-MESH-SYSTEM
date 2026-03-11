#pragma once
#include <Arduino.h>

#define QUEUE_SIZE 20

struct SensorData {
    float temp;
    float humidity;
};

class GatewayQueue {
public:
    static void begin();
    static bool enqueue(float temp, float humidity);
    static void process();
    static uint8_t getSize();
};
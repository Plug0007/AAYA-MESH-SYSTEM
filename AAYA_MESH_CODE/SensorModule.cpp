#include "SensorModule.h"
#include <Arduino.h>

#define LM35_PIN 16

void SensorModule::begin() {

    analogReadResolution(12);
    analogSetPinAttenuation(LM35_PIN, ADC_11db);
}

void SensorModule::buildPayload(uint8_t* buffer, uint8_t& length) {

    int raw = analogRead(LM35_PIN);

    float voltage = raw * (3.3 / 4095.0);
    float temperature = voltage * 100.0;

    float humidity = 0;   // placeholder for future sensor

    memcpy(buffer, &temperature, sizeof(float));
    memcpy(buffer + sizeof(float), &humidity, sizeof(float));

    length = sizeof(float) * 2;
}
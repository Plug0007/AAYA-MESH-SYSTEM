#include "LoRaManager.h"
#include <SPI.h>
#include <RadioLib.h>

#define LORA_CS    7
#define LORA_DIO1  33
#define LORA_BUSY  34
#define LORA_RST   8

#define LORA_SCK   5
#define LORA_MISO  3
#define LORA_MOSI  6

#define LORA_FREQ  868.0

SPIClass spi(HSPI);
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, spi);

// ================= INIT =================
bool LoRaManager::begin() {

    spi.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    pinMode(LORA_RST, OUTPUT);
    digitalWrite(LORA_RST, LOW);
    delay(10);
    digitalWrite(LORA_RST, HIGH);
    delay(10);

    radio.setTCXO(1.8, 5);

    int state = radio.begin(LORA_FREQ);

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa INIT FAILED: ");
        Serial.println(state);
        return false;
    }

    Serial.println("LoRa INIT SUCCESS");

    radio.setSpreadingFactor(7);
    radio.setBandwidth(125.0);
    radio.setCodingRate(5);
    radio.setOutputPower(17);
    radio.setCurrentLimit(140);

    radio.setCRC(false);
    radio.explicitHeader();

    radio.startReceive();

    return true;
}
// ================= SEND =================
bool LoRaManager::send(uint8_t* data, size_t length) {

    // Leave RX mode before transmit
    radio.standby();

    int state = radio.transmit(data, length);

    if (state != RADIOLIB_ERR_NONE) {

        Serial.print("LoRa TX Failed: ");
        Serial.println(state);

        // Restart RX even on failure
        radio.startReceive();
        return false;
    }

    // Return to RX mode
    radio.startReceive();

    return true;
}

// ================= RECEIVE =================
bool LoRaManager::receive(uint8_t* buffer, size_t& length) {

    int state = radio.readData(buffer, length);

    if (state == RADIOLIB_ERR_NONE) {
        return true;
    }

    return false;
}
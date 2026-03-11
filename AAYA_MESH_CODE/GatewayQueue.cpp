#include "GatewayQueue.h"
#include "MQTTGateway.h"
#include <SPIFFS.h>

static SensorData queueBuffer[QUEUE_SIZE];
static uint8_t head = 0;
static uint8_t tail = 0;
static uint8_t count = 0;

#define QUEUE_FILE "/queue.dat"

// ================= SAVE TO FLASH =================
void saveToFlash() {

    File file = SPIFFS.open(QUEUE_FILE, FILE_WRITE);
    if (!file) return;

    file.write((uint8_t*)&head, sizeof(head));
    file.write((uint8_t*)&tail, sizeof(tail));
    file.write((uint8_t*)&count, sizeof(count));
    file.write((uint8_t*)queueBuffer, sizeof(queueBuffer));

    file.close();
}

// ================= LOAD FROM FLASH =================
void loadFromFlash() {

    if (!SPIFFS.exists(QUEUE_FILE)) return;

    File file = SPIFFS.open(QUEUE_FILE, FILE_READ);
    if (!file) return;

    file.read((uint8_t*)&head, sizeof(head));
    file.read((uint8_t*)&tail, sizeof(tail));
    file.read((uint8_t*)&count, sizeof(count));
    file.read((uint8_t*)queueBuffer, sizeof(queueBuffer));

    file.close();

    Serial.print("Queue restored | Size: ");
    Serial.println(count);
}

// ================= INIT =================
void GatewayQueue::begin() {
    head = 0;
    tail = 0;
    count = 0;
    loadFromFlash();
}

// ================= ENQUEUE =================
bool GatewayQueue::enqueue(float temp, float humidity) {

    if (count >= QUEUE_SIZE) {
        Serial.println("Queue Full - Dropping Data");
        return false;
    }

    queueBuffer[tail].temp = temp;
    queueBuffer[tail].humidity = humidity;

    tail = (tail + 1) % QUEUE_SIZE;
    count++;

    saveToFlash();

    Serial.print("Enqueued | Queue size: ");
    Serial.println(count);

    return true;
}

// ================= PROCESS =================
void GatewayQueue::process() {

    if (count == 0) return;
    if (!MQTTGateway::isConnected()) return;

    SensorData &data = queueBuffer[head];

    if (MQTTGateway::publishSensor(data.temp, data.humidity)) {

        head = (head + 1) % QUEUE_SIZE;
        count--;

        saveToFlash();

        Serial.println("Packet flushed from queue");

        if (count == 0) {
            Serial.println("Queue Empty");
        }
    }
}

uint8_t GatewayQueue::getSize() {
    return count;
}
#include "PacketManager.h"
#include "LoRaManager.h"
#include "RoleController.h"
#include "SensorModule.h"
#include "GatewayQueue.h"

static uint16_t currentPacketID = 0;
static uint16_t lastAckProcessed = 0;

// ================= ACK STATE =================
static bool waitingForAck = false;
static uint16_t waitingPacketID = 0;
static unsigned long ackTimeout = 0;
static uint8_t retryCount = 0;

#define ACK_TIMEOUT_MS 3000
#define MAX_RETRIES 3
#define ACTUATOR_PIN 48

// ================= DUPLICATE BUFFER =================
struct PacketHistory {
    uint32_t senderID;
    uint16_t packetID;
};

static PacketHistory recentPackets[PACKET_HISTORY_SIZE];
static uint8_t historyIndex = 0;

// ================= CHECKSUM =================
uint16_t calculateChecksum(uint8_t* data, size_t length) {
    uint16_t sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

// ================= DUPLICATE CHECK =================
bool isDuplicate(uint32_t senderID, uint16_t packetID) {
    for (int i = 0; i < PACKET_HISTORY_SIZE; i++) {
        if (recentPackets[i].senderID == senderID &&
            recentPackets[i].packetID == packetID) {
            return true;
        }
    }
    return false;
}

void storePacket(uint32_t senderID, uint16_t packetID) {
    recentPackets[historyIndex].senderID = senderID;
    recentPackets[historyIndex].packetID = packetID;
    historyIndex = (historyIndex + 1) % PACKET_HISTORY_SIZE;
}

// ================= INIT =================
void PacketManager::begin() {

    Serial.println("Starting LoRa...");

    if (!LoRaManager::begin()) {
        Serial.println("LoRa FAILED");
    } else {
        Serial.println("LoRa READY");
    }

    Serial.println("Packet Manager Ready");

    if (RoleController::getRole() == ROLE_COLLECTOR) {
        pinMode(ACTUATOR_PIN, OUTPUT);
        digitalWrite(ACTUATOR_PIN, LOW);
    }
}
// ================= UPDATE LOOP =================
void PacketManager::update() {

    static unsigned long lastSend = 0;

    // ================= COLLECTOR SEND =================
    if (RoleController::getRole() == ROLE_COLLECTOR) {

        if (!waitingForAck && millis() - lastSend > 5000) {

            MeshPacket packet;
            memset(&packet, 0, sizeof(MeshPacket));

            packet.networkID = NETWORK_ID;
            packet.packetType = PACKET_SENSOR_DATA;
            packet.senderID = (uint32_t)(ESP.getEfuseMac() >> 32);
            packet.packetID = currentPacketID++;
            packet.hopCount = 0;
            packet.maxHops = 5;

            SensorModule::buildPayload(packet.payload, packet.payloadLength);

            packet.checksum = calculateChecksum(
                (uint8_t*)&packet,
                sizeof(MeshPacket) - sizeof(packet.checksum)
            );

            if (LoRaManager::send((uint8_t*)&packet, sizeof(MeshPacket))) {

                Serial.println("Packet Sent - Waiting ACK");

                waitingForAck = true;
                waitingPacketID = packet.packetID;
                ackTimeout = millis();
                retryCount = 0;
            }

            lastSend = millis();
        }

        // ACK timeout
        if (waitingForAck && millis() - ackTimeout > ACK_TIMEOUT_MS) {

            if (retryCount < MAX_RETRIES) {
                Serial.println("ACK Timeout - Retrying");
                waitingForAck = false;
                retryCount++;
            } else {
                Serial.println("ACK Failed - Dropping Packet");
                waitingForAck = false;
            }
        }
    }

    // ================= RECEIVE =================
    uint8_t buffer[sizeof(MeshPacket)];
    size_t length = sizeof(buffer);

    if (!LoRaManager::receive(buffer, length)) return;
    if (length != sizeof(MeshPacket)) return;

    MeshPacket* received = (MeshPacket*)buffer;

    if (received->networkID != NETWORK_ID) return;

    uint16_t calculated = calculateChecksum(
        buffer,
        sizeof(MeshPacket) - sizeof(received->checksum)
    );

    if (calculated != received->checksum) return;

    // ================= COLLECTOR SIDE =================
    if (RoleController::getRole() == ROLE_COLLECTOR) {

        if (waitingForAck &&
            received->packetType == PACKET_ACK &&
            received->packetID == waitingPacketID &&
            received->packetID != lastAckProcessed) {

            Serial.println("ACK Received");
            lastAckProcessed = received->packetID;
            waitingForAck = false;
        }

        // Actuator command
        if (received->packetType == PACKET_ACTUATOR_COMMAND) {

            char cmd[received->payloadLength + 1];
            memcpy(cmd, received->payload, received->payloadLength);
            cmd[received->payloadLength] = '\0';

            Serial.print("Actuator Command: ");
            Serial.println(cmd);

            if (strcmp(cmd, "LED_ON") == 0) {
                digitalWrite(ACTUATOR_PIN, HIGH);
                Serial.println("LED ON");
            }
            else if (strcmp(cmd, "LED_OFF") == 0) {
                digitalWrite(ACTUATOR_PIN, LOW);
                Serial.println("LED OFF");
            }
        }

        return;
    }

    // ================= SINK SIDE =================

    if (isDuplicate(received->senderID, received->packetID)) return;
    storePacket(received->senderID, received->packetID);

    if (received->packetType == PACKET_ACK) return;

    if (received->packetType == PACKET_SENSOR_DATA) {

        float temp, humidity;
        memcpy(&temp, received->payload, sizeof(float));
        memcpy(&humidity, received->payload + sizeof(float), sizeof(float));

        Serial.println("Sensor Data Packet");
        Serial.print("Temp: ");
        Serial.println(temp);
        Serial.print("Humidity: ");
        Serial.println(humidity);

        GatewayQueue::enqueue(temp, humidity);

        // Send ACK
        MeshPacket ack;
        memset(&ack, 0, sizeof(MeshPacket));

        ack.networkID = NETWORK_ID;
        ack.packetType = PACKET_ACK;
        ack.senderID = (uint32_t)(ESP.getEfuseMac() >> 32);
        ack.packetID = received->packetID;

        ack.checksum = calculateChecksum(
            (uint8_t*)&ack,
            sizeof(MeshPacket) - sizeof(ack.checksum)
        );

        LoRaManager::send((uint8_t*)&ack, sizeof(MeshPacket));
        Serial.println("ACK Sent");
    }
}

// ================= SEND ACTUATOR FROM SINK =================
void PacketManager::sendActuatorCommand(String command) {

    if (RoleController::getRole() != ROLE_SINK) return;

    MeshPacket packet;
    memset(&packet, 0, sizeof(MeshPacket));

    packet.networkID = NETWORK_ID;
    packet.packetType = PACKET_ACTUATOR_COMMAND;
    packet.senderID = (uint32_t)(ESP.getEfuseMac() >> 32);
    packet.packetID = currentPacketID++;
    packet.hopCount = 0;
    packet.maxHops = 5;

    packet.payloadLength = command.length();
    memcpy(packet.payload, command.c_str(), packet.payloadLength);

    packet.checksum = calculateChecksum(
        (uint8_t*)&packet,
        sizeof(MeshPacket) - sizeof(packet.checksum)
    );

    LoRaManager::send((uint8_t*)&packet, sizeof(MeshPacket));

    Serial.println("Actuator Command Sent to Mesh");
}
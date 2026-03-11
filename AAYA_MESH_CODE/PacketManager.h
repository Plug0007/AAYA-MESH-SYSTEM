#pragma once
#include <Arduino.h>

#define MAX_PAYLOAD_SIZE 64
#define NETWORK_ID 0x01
#define PACKET_HISTORY_SIZE 10

#pragma pack(push, 1)
struct MeshPacket {
    uint8_t  networkID;
    uint8_t packetType;//mod 5 
    uint32_t senderID;
    uint16_t packetID;
    uint8_t  hopCount;
    uint8_t  maxHops;
    uint8_t  payloadLength;
    uint8_t  payload[MAX_PAYLOAD_SIZE];
    uint16_t checksum;
};
#pragma pack(pop)

class PacketManager {
public:
    static void begin();
    static void update();
    static void sendActuatorCommand(String command);
};
//mod 5
enum PacketType {
    PACKET_SENSOR_DATA = 1,
    PACKET_ACTUATOR_COMMAND = 2,
    PACKET_CONFIG = 3,
    PACKET_ACK = 4
};


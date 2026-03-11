#include "NodeIdentity.h"

String NodeIdentity::getNodeID() {
    uint64_t chipid = ESP.getEfuseMac();
    return String((uint32_t)(chipid >> 32), HEX);
}

String NodeIdentity::getFirmwareVersion() {
    return "1.0.0";
}

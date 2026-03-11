#pragma once
#include <Arduino.h>

class NodeIdentity {
public:
    static String getNodeID();
    static String getFirmwareVersion();
};

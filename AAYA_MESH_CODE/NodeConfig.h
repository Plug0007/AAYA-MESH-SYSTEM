#pragma once
#include <Arduino.h>

enum NodeRole {
    ROLE_COLLECTOR,
    ROLE_SINK,
    ROLE_RELAY
};

struct NodeConfig {
    NodeRole role;
    uint32_t sendInterval;
};

class ConfigManager {
public:
    static NodeConfig load();
};
#include "NodeConfig.h"

NodeConfig ConfigManager::load() {
    NodeConfig config;

    //config.role = ROLE_COLLECTOR;   // Change per board
    config.role = ROLE_SINK;

    config.sendInterval = 5000;

    return config;
}

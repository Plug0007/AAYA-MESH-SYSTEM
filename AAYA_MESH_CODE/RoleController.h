#pragma once
#include "NodeConfig.h"

class RoleController {
public:
    static void begin(NodeConfig config);
    static NodeRole getRole();
};

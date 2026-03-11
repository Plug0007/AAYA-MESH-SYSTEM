#include "RoleController.h"

static NodeRole currentRole;

void RoleController::begin(NodeConfig config) {
    currentRole = config.role;
}

NodeRole RoleController::getRole() {
    return currentRole;
}

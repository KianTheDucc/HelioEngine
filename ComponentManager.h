#pragma once
#include "Components.h"
#include <unordered_map>

class ComponentManager {
public:
    std::unordered_map<Entity, Position> positions;
    std::unordered_map<Entity, Velocity> velocities;
    std::unordered_map<Entity, Renderable> renderables;
    std::unordered_map<Entity, PlayerControlled> playerControlled;
    std::unordered_map<Entity, uint32_t> componentMasks;
};

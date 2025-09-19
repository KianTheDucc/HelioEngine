#pragma once
#include "Components.h"
#include <queue>
#include <set>

class EntityManager {
public:
    EntityManager(Entity maxEntities = 5000)
        : maxEntities(maxEntities)
    {
        // Initialize the free ID queue
        for (Entity e = 1; e <= maxEntities; ++e) {
            freeIDs.push(e);
        }
    }

    // Create a new entity and return its ID
    Entity createEntity() {
        if (freeIDs.empty()) return 0; // 0 = invalid entity
        Entity id = freeIDs.front();
        freeIDs.pop();
        activeEntities.insert(id);
        return id;
    }

    // Destroy an entity
    void destroyEntity(Entity entity) {
        activeEntities.erase(entity);
        freeIDs.push(entity);
    }

    // Optional: check if an entity is alive
    bool isAlive(Entity entity) const {
        return activeEntities.find(entity) != activeEntities.end();
    }

private:
    Entity maxEntities;
    std::queue<Entity> freeIDs;
    std::set<Entity> activeEntities;
};
#pragma once
#include <SDL.h>
#include "ComponentManager.h"
#include "Systems.h"
#include "EntityManager.h"

class Game {
public:
    Game() = default;
    ~Game() = default;

    bool init(const char* title, int width, int height);
    void handleEvents();
    void update(float dt);
    void render();
    void clean();
    bool running() const { return isRunning; }

    Entity spawnEntity(float x, float y, SDL_Color color, bool playerControlled = false) {
        Entity e = entityManager.createEntity();

        cm.positions[e] = { x, y };
        cm.renderables[e] = { color, 50, 50 };
        cm.velocities[e] = { 0.0f, 0.0f };

        uint32_t mask = 0;
        mask |= POSITION | VELOCITY | RENDERABLE;

        if (playerControlled) {
            cm.playerControlled[e] = {};
            mask |= PLAYERCONTROLLED;
        }

        cm.componentMasks[e] = mask;
        return e;
    }

private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    ComponentManager cm;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    EntityManager entityManager;

    Entity player;
    Entity blueBox;
    Entity enemy = 0;

};
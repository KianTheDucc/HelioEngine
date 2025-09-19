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

    // Fully mask-integrated spawn
    Entity spawnEntity(float x, float y, SDL_Color color, bool playerControlled = false);
    void destroyEntityWithComponents(Entity e);
    void destroyAllEnemies();
    void resetCollisionColors() {
        for (auto& [entity, rend] : cm.renderables) {
            rend.color = rend.color.r == 255 && rend.color.g == 255 && rend.color.b == 0 ? SDL_Color{ 0, 255, 0, 255 } : rend.color;
        }
    }

private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    ComponentManager cm;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;
    EntityManager entityManager;
    EnemyAISystem enemyAISystem;
    HealthSystem healthSystem;

    Entity player;
    Entity blueBox;
    Entity enemy = 0;
    std::vector<Entity> enemies; // track multiple dynamic enemies


};
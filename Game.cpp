#include "Game.h"
#include <iostream>

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Spawn initial entities
    player = spawnEntity(100.0f, 100.0f, SDL_Color{ 255, 0, 0, 255 }, true);
    blueBox = spawnEntity(300.0f, 200.0f, SDL_Color{ 0, 0, 255, 255 });

    isRunning = true;
    return true;
}
Entity Game::spawnEntity(float x, float y, SDL_Color color, bool playerControlled) {
    Entity e = entityManager.createEntity();
    if (e == 0) return 0;

    // Position & movement
    cm.positions[e] = { x, y };
    cm.velocities[e] = { 0.0f, 0.0f };

    // Rendering
    cm.renderables[e] = { color, 50, 50 };

    // Health
    if (playerControlled) {
        cm.healths[e] = { 100, 100 }; // player starts with more HP
    }
    else {
        cm.healths[e] = { 20, 20 };   // enemies start with less HP
    }

    //Damage Cooldown

    cm.damageCooldown[e] = { 0.0f, 1.0f };

    // Component mask
    uint32_t mask = POSITION | VELOCITY | RENDERABLE | HEALTH;
    if (playerControlled) {
        cm.playerControlled[e] = {};
        mask |= PLAYERCONTROLLED;
    }
    cm.componentMasks[e] = mask;

    return e;
}
Entity Game::spawnProjectile(float x, float y, float vx, float vy, SDL_Color color) {
    Entity e = entityManager.createEntity();
    if (e == 0) return 0;

    cm.positions[e] = { x, y };
    cm.velocities[e] = { vx, vy };
    cm.renderables[e] = { color, 10, 10 };  // small square bullet
    cm.projectiles[e] = { 10, 2.0f };       // damage = 10, lifetime = 2 seconds

    cm.componentMasks[e] = POSITION | VELOCITY | RENDERABLE | PROJECTILE;
    return e;
}
void Game::destroyEntityWithComponents(Entity e) {
    if (e == 0) return;
    entityManager.destroyEntity(e);
    cm.positions.erase(e);
    cm.velocities.erase(e);
    cm.renderables.erase(e);
    cm.healths.erase(e);
    cm.damageCooldown.erase(e);
    cm.playerControlled.erase(e);
    cm.componentMasks.erase(e);
}
void Game::destroyAllEnemies() {
    for (Entity e : enemies)
        destroyEntityWithComponents(e);
    enemies.clear();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        inputSystem.handleEvent(event, cm);

        // Press E to spawn an enemy
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) {
            float x = rand() % 700 + 50;
            float y = rand() % 500 + 50;
            Entity enemy = spawnEntity(x, y, SDL_Color{ 0, 255, 0, 255 });
            cm.velocities[enemy] = { static_cast<float>(rand() % 200 - 100),
                                     static_cast<float>(rand() % 200 - 100) };
            if (enemy != 0) enemies.push_back(enemy);
        }

        // Press DELETE to remove the last spawned enemy
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE) {
            if (!enemies.empty()) {
                Entity e = enemies.back();
                destroyEntityWithComponents(e);
                enemies.pop_back();
            }
        }

        // Press SPACE to shoot a bullet
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            for (auto& [entity, _] : cm.playerControlled) {
                auto& pos = cm.positions[entity];
                spawnProjectile(pos.x + 20, pos.y - 10, 0.0f, -300.0f, { 255, 255, 0, 255 });
            }
        }
    }
}
void updateDamageCooldowns(ComponentManager& cm, float dt) {
    for (auto& [entity, dc] : cm.damageCooldown) {
        if (dc.timer > 0.0f) {
            dc.timer -= dt;
            if (dc.timer < 0.0f) dc.timer = 0.0f;
        }
    }
}
void Game::update(float dt) {
    std::vector<Entity> active = entityManager.getActiveEntities();

    resetCollisionColors();
    movementSystem.update(cm, active, dt);

    enemyAISystem.update(cm, active, player, dt); // AI updates velocities


    for (Entity e : active) {
        auto& pos = cm.positions[e];
        auto& vel = cm.velocities[e];
        auto& rend = cm.renderables[e];

        // If projectile, destroy when out of bounds
        if (cm.componentMasks[e] & PROJECTILE) {
            if (pos.x < 0 || pos.x + rend.w > 800 ||
                pos.y < 0 || pos.y + rend.h > 600) {
                destroyEntityWithComponents(e);
            }
            continue; // skip bounce logic
        }

        // Normal entities bounce
        if (pos.x < 0) { pos.x = 0; vel.x = -vel.x; }
        if (pos.x + rend.w > 800) { pos.x = 800 - rend.w; vel.x = -vel.x; }

        if (pos.y < 0) { pos.y = 0; vel.y = -vel.y; }
        if (pos.y + rend.h > 600) { pos.y = 600 - rend.h; vel.y = -vel.y; }
    }


    updateDamageCooldowns(cm, dt);
    collisionSystem.update(cm, active, healthSystem);
    projectileSystem.update(cm, active, dt);
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    auto active = entityManager.getActiveEntities();
    renderSystem.render(cm, renderer, active);

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
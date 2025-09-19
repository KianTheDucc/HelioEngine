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

    // Create Player (red square, controllable)
    player = spawnEntity(100.0f, 100.0f, SDL_Color{ 255, 0, 0, 255 }, true);

    // Create Blue Box (static)
    blueBox = spawnEntity(300.0f, 200.0f, SDL_Color{ 0, 0, 255, 255 });

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        inputSystem.handleEvent(event, cm);

        // Example: press SPACE to spawn the enemy
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            if (enemy == 0) { // not already spawned
                enemy = spawnEntity(400.0f, 300.0f, SDL_Color{ 0, 255, 0, 255 }, true);
            }
        }

        // Example: press DELETE to remove the enemy
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE) {
            if (enemy != 0) { // enemy exists
                entityManager.destroyEntity(enemy);
                cm.positions.erase(enemy);
                cm.velocities.erase(enemy);
                cm.renderables.erase(enemy);
                cm.playerControlled.erase(enemy);
                cm.componentMasks.erase(enemy);
                enemy = 0; // reset
            }
        }
    }
    
}

void Game::update(float dt) {
    auto active = entityManager.getActiveEntities();
    movementSystem.update(cm, active, dt);
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
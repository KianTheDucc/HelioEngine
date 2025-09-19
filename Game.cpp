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
    player = 1;
    cm.positions[player] = { 100.0f, 100.0f };
    cm.velocities[player] = { 0.0f, 0.0f };
    cm.renderables[player] = { SDL_Color{255, 0, 0, 255}, 50, 50 };
    cm.playerControlled[player] = {};

    // Create Blue Box (static)
    blueBox = 2;
    cm.positions[blueBox] = { 300.0f, 200.0f };
    cm.renderables[blueBox] = { SDL_Color{0, 0, 255, 255}, 50, 50 };

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        inputSystem.handleEvent(event, cm);
    }
}

void Game::update(float dt) {
    movementSystem.update(cm, dt);
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderSystem.render(cm, renderer);

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
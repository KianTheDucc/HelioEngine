#pragma once
#include <SDL.h>
#include "ComponentManager.h"
#include "Systems.h"

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

private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    ComponentManager cm;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;

    Entity player;
    Entity blueBox;
};
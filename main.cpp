#include "Game.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("ECS 2D Engine Demo", 800, 600)) return -1;

    Uint32 lastTick = SDL_GetTicks();

    while (game.running()) {
        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 1000.0f; // delta time in seconds
        lastTick = currentTick;

        game.handleEvents();
        game.update(dt);  // pass delta-time
        game.render();

        SDL_Delay(1); // prevent 100% CPU
    }

    game.clean();
    return 0;
}
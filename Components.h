#pragma once
#include <SDL.h>
#include <cstdint>

using Entity = std::uint32_t;

struct Position { float x, y; };      // float for smooth movement
struct Velocity { float x, y; };      // pixels per second
struct Renderable { SDL_Color color; int w, h; };
struct PlayerControlled {};           // tag component
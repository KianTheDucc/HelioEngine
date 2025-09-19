#pragma once
#include <SDL.h>
#include <cstdint>

using Entity = std::uint32_t;

struct Position { float x, y; };      // float for smooth movement
struct Velocity { float x, y; };      // pixels per second
struct Renderable { SDL_Color color; int w, h; };
struct PlayerControlled {};           // tag component
struct Health {
    int current;
    int max;
};


// Component bitmask identifiers
enum ComponentType : uint32_t {
    POSITION = 1 << 0,        // 0001
    VELOCITY = 1 << 1,        // 0010
    RENDERABLE = 1 << 2,      // 0100
    PLAYERCONTROLLED = 1 << 3, // 1000
    HEALTH = 1 << 4

};
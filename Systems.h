#pragma once
#include "ComponentManager.h"
#include <SDL.h>

class InputSystem {
public:
    void handleEvent(const SDL_Event& event, ComponentManager& cm) {
        for (auto& [entity, _] : cm.playerControlled) {
            auto& vel = cm.velocities[entity];

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:    vel.y = -200.0f; break;
                case SDLK_DOWN:  vel.y = 200.0f; break;
                case SDLK_LEFT:  vel.x = -200.0f; break;
                case SDLK_RIGHT: vel.x = 200.0f; break;
                }
            }
            else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:  vel.y = 0.0f; break;
                case SDLK_LEFT:
                case SDLK_RIGHT: vel.x = 0.0f; break;
                }
            }
        }
    }
};

class MovementSystem {
public:
    void update(ComponentManager& cm, float dt) {
        for (auto& [entity, pos] : cm.positions) {
            if (cm.velocities.find(entity) != cm.velocities.end()) {
                auto& vel = cm.velocities[entity];
                pos.x += vel.x * dt;
                pos.y += vel.y * dt;
            }
        }
    }
};

class RenderSystem {
public:
    void render(ComponentManager& cm, SDL_Renderer* renderer) {
        for (auto& [entity, rend] : cm.renderables) {
            if (cm.positions.find(entity) != cm.positions.end()) {
                auto& pos = cm.positions[entity];
                SDL_Rect rect = {
                    static_cast<int>(pos.x),
                    static_cast<int>(pos.y),
                    rend.w,
                    rend.h
                };
                SDL_SetRenderDrawColor(renderer, rend.color.r, rend.color.g, rend.color.b, rend.color.a);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
};
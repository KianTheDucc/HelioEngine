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
    void update(ComponentManager& cm, const std::vector<Entity>& entities, float dt) {
        const uint32_t requiredMask = POSITION | VELOCITY;

        for (Entity entity : entities) {
            if ((cm.componentMasks[entity] & requiredMask) == requiredMask) {
                auto& pos = cm.positions[entity];
                auto& vel = cm.velocities[entity];
                pos.x += vel.x * dt;
                pos.y += vel.y * dt;
            }
        }
    }
};

class RenderSystem {
public:
    void render(ComponentManager& cm, SDL_Renderer* renderer, const std::vector<Entity>& entities) {
        const uint32_t requiredMask = POSITION | RENDERABLE;

        for (Entity entity : entities) {
            if ((cm.componentMasks[entity] & requiredMask) == requiredMask) {
                auto& pos = cm.positions[entity];
                auto& rend = cm.renderables[entity];

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
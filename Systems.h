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
class CollisionSystem {
public:
    // Check collisions between entities with POSITION and RENDERABLE
    void update(ComponentManager& cm, const std::vector<Entity>& entities) {
        const uint32_t requiredMask = POSITION | RENDERABLE;

        for (size_t i = 0; i < entities.size(); ++i) {
            Entity a = entities[i];
            if ((cm.componentMasks[a] & requiredMask) != requiredMask) continue;

            auto& posA = cm.positions[a];
            auto& rendA = cm.renderables[a];

            SDL_Rect rectA = {
                static_cast<int>(posA.x),
                static_cast<int>(posA.y),
                rendA.w,
                rendA.h
            };

            for (size_t j = i + 1; j < entities.size(); ++j) {
                Entity b = entities[j];
                if ((cm.componentMasks[b] & requiredMask) != requiredMask) continue;

                auto& posB = cm.positions[b];
                auto& rendB = cm.renderables[b];

                SDL_Rect rectB = {
                    static_cast<int>(posB.x),
                    static_cast<int>(posB.y),
                    rendB.w,
                    rendB.h
                };

                if (SDL_HasIntersection(&rectA, &rectB)) {
                    onCollision(a, b, cm);
                }
            }
        }
    }

    // Simple collision reaction (change color on collision)
    void onCollision(Entity a, Entity b, ComponentManager& cm) {
        auto& velA = cm.velocities[a];
        auto& velB = cm.velocities[b];

        // Simple AABB bounce: reverse velocity
        velA.x = -velA.x;
        velA.y = -velA.y;

        velB.x = -velB.x;
        velB.y = -velB.y;

        // Optional: change color to visualize collision
        //if ((cm.componentMasks[a] & RENDERABLE) == RENDERABLE)
        //    cm.renderables[a].color = SDL_Color{ 255, 255, 0, 255 };
        //if ((cm.componentMasks[b] & RENDERABLE) == RENDERABLE)
        //    cm.renderables[b].color = SDL_Color{ 255, 255, 0, 255 };
    }
};
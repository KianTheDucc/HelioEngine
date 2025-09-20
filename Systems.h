#pragma once
#include "ComponentManager.h"
#include "EntityManager.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>

// ------------------- HEALTH SYSTEM -------------------
class HealthSystem {
public:
    void damage(Entity e, ComponentManager& cm, int amount) {
        if ((cm.componentMasks[e] & HEALTH) != HEALTH) return;

        // check if entity has a cooldown component
        if (cm.damageCooldown.find(e) != cm.damageCooldown.end()) {
            auto& dc = cm.damageCooldown[e];
            if (dc.timer > 0.0f) return; // still on cooldown
            dc.timer = dc.cooldown;      // reset timer
        }

        auto& h = cm.healths[e];
        h.current -= amount;
        if (h.current <= 0) {
            h.current = 0;
            onDeath(e, cm);
        }
    }

    void heal(Entity e, ComponentManager& cm, int amount) {
        if ((cm.componentMasks[e] & HEALTH) != HEALTH) return;

        auto& h = cm.healths[e];
        h.current = std::min(h.current + amount, h.max);
    }

private:
    void onDeath(Entity e, ComponentManager& cm) {
        cm.componentMasks[e] = 0;
        cm.positions.erase(e);
        cm.velocities.erase(e);
        cm.renderables.erase(e);
        cm.healths.erase(e);
    }
};

// ------------------- INPUT SYSTEM -------------------
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

// ------------------- MOVEMENT SYSTEM -------------------
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

// ------------------- RENDER SYSTEM -------------------
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

// ------------------- COLLISION SYSTEM -------------------
class CollisionSystem {
public:
    void update(ComponentManager& cm, const std::vector<Entity>& entities, HealthSystem& healthSystem) {
        const uint32_t requiredMask = POSITION | RENDERABLE | VELOCITY;

        for (size_t i = 0; i < entities.size(); ++i) {
            Entity a = entities[i];
            if ((cm.componentMasks[a] & requiredMask) != requiredMask) continue;

            auto& posA = cm.positions[a];
            auto& rendA = cm.renderables[a];
            auto& velA = cm.velocities[a];

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
                auto& velB = cm.velocities[b];

                SDL_Rect rectB = {
                    static_cast<int>(posB.x),
                    static_cast<int>(posB.y),
                    rendB.w,
                    rendB.h
                };

                if (SDL_HasIntersection(&rectA, &rectB)) {
                    resolveCollision(posA, velA, rendA, posB, velB, rendB);

                    if (cm.playerControlled.find(a) != cm.playerControlled.end()) {
                        healthSystem.damage(a, cm, 10);
                    }
                    if (cm.playerControlled.find(b) != cm.playerControlled.end()) {
                        healthSystem.damage(b, cm, 10);
                    }
                }
            }
        }
    }

private:
    void resolveCollision(Position& posA, Velocity& velA, Renderable& rendA,
        Position& posB, Velocity& velB, Renderable& rendB)
    {
        float ax1 = posA.x, ay1 = posA.y;
        float ax2 = posA.x + rendA.w, ay2 = posA.y + rendA.h;
        float bx1 = posB.x, by1 = posB.y;
        float bx2 = posB.x + rendB.w, by2 = posB.y + rendB.h;

        float overlapX = std::min(ax2, bx2) - std::max(ax1, bx1);
        float overlapY = std::min(ay2, by2) - std::max(ay1, by1);

        if (overlapX < overlapY) {
            if (ax1 < bx1) { posA.x -= overlapX / 2; posB.x += overlapX / 2; }
            else { posA.x += overlapX / 2; posB.x -= overlapX / 2; }
            velA.x = -velA.x * 0.5f;
            velB.x = -velB.x * 0.5f;
        }
        else {
            if (ay1 < by1) { posA.y -= overlapY / 2; posB.y += overlapY / 2; }
            else { posA.y += overlapY / 2; posB.y -= overlapY / 2; }
            velA.y = -velA.y * 0.5f;
            velB.y = -velB.y * 0.5f;
        }
    }
};

// ------------------- ENEMY AI SYSTEM -------------------
class EnemyAISystem {
public:
    void update(ComponentManager& cm, const std::vector<Entity>& entities, Entity player, float dt) {
        const uint32_t requiredMask = POSITION | VELOCITY;

        if ((cm.componentMasks[player] & POSITION) != POSITION) return;
        auto& playerPos = cm.positions[player];

        for (Entity e : entities) {
            if ((cm.componentMasks[e] & requiredMask) != requiredMask) continue;
            if (e == player) continue;

            auto& pos = cm.positions[e];
            auto& vel = cm.velocities[e];

            float dx = playerPos.x - pos.x;
            float dy = playerPos.y - pos.y;
            float length = sqrtf(dx * dx + dy * dy);

            if (length > 0.0f) {
                float speed = 100.0f;
                vel.x = (dx / length) * speed;
                vel.y = (dy / length) * speed;
            }
        }
    }
};
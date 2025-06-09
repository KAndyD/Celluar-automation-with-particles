#pragma once

#include <vector>
#include <cmath>
#include <random>
#include "particle.hpp"
#include "config.hpp"

constexpr float GROUP_ATTRACT_STRENGTH = 0.1f;      // сильное притяжение для одного типа
constexpr float GROUP_REPEL_STRENGTH = 0.02f;       // слабое отталкивание для разных типов
constexpr float MIN_DISTANCE = 1e-3f;
constexpr float MAX_SPEED = 0.5f;
constexpr float PARTICLE_RADIUS = 0.5f;

inline void init_group(std::vector<Particle>& particles, int count, int width, int height) {
    particles.clear();
    particles.reserve(count);

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(0.f, float(width - 1));
    std::uniform_real_distribution<float> distY(0.f, float(height - 1));
    std::uniform_int_distribution<int> distType(0, TYPE_COUNT - 1);

    for (int i = 0; i < count; ++i) {
        particles.push_back(Particle{
            distX(rng),
            distY(rng),
            0.f,
            0.f,
            distType(rng),
            1.f,
            0,
            i
        });
    }
}

inline void update_group(std::vector<Particle>& particles, int width, int height) {
    for (auto& p : particles) {
        float forceX = 0.f;
        float forceY = 0.f;

        for (const auto& other : particles) {
            if (&p == &other) continue;

            float dx = other.x - p.x;
            float dy = other.y - p.y;
            float distSq = dx * dx + dy * dy;

            if (distSq < MIN_DISTANCE) distSq = MIN_DISTANCE;

            float dist = std::sqrt(distSq);
            float nx = dx / dist;
            float ny = dy / dist;

            float invDistSq = 1.f / distSq;

            if (p.type == other.type) {
                // сильное притяжение частиц одного типа (обратно пропорционально расстоянию в квадрате)
                forceX += GROUP_ATTRACT_STRENGTH * nx * invDistSq;
                forceY += GROUP_ATTRACT_STRENGTH * ny * invDistSq;
            } else {
                // слабое отталкивание частиц разных типов
                forceX -= GROUP_REPEL_STRENGTH * nx * invDistSq;
                forceY -= GROUP_REPEL_STRENGTH * ny * invDistSq;
            }

            // Отталкивание, если частицы слишком близко (чтобы избежать наложения)
            if (dist < 2 * PARTICLE_RADIUS) {
                float overlap = 2 * PARTICLE_RADIUS - dist;
                forceX -= GROUP_REPEL_STRENGTH * nx * overlap * 10.f;
                forceY -= GROUP_REPEL_STRENGTH * ny * overlap * 10.f;
            }
        }

        p.vx += forceX;
        p.vy += forceY;

        float speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        if (speed > MAX_SPEED) {
            p.vx = (p.vx / speed) * MAX_SPEED;
            p.vy = (p.vy / speed) * MAX_SPEED;
        }
    }

    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;

        if (p.x < 0) { p.x = 0; p.vx = -p.vx; }
        if (p.y < 0) { p.y = 0; p.vy = -p.vy; }
        if (p.x > width - 1) { p.x = width - 1; p.vx = -p.vx; }
        if (p.y > height - 1) { p.y = height - 1; p.vy = -p.vy; }
    }
}

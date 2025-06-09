#include "simulation.hpp"
#include "config.hpp"
#include <cstdlib>
#include <cmath>
#include <random>
#include <unordered_set>

extern std::array<std::array<float, TYPE_COUNT>, TYPE_COUNT> interactionMatrix;

void reset_particles(std::vector<Particle>& particles, int count, int width, int height) {
    particles.clear();
    particles.reserve(count);

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(0, width);
    std::uniform_real_distribution<float> distY(0, height);
    std::uniform_int_distribution<int> distType(0, TYPE_COUNT - 1);
    std::uniform_real_distribution<float> distMass(1.0f, 1.5f);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = distX(rng);
        p.y = distY(rng);
        p.vx = 0.0f;
        p.vy = 0.0f;
        p.type = distType(rng);
        p.mass = distMass(rng);
        p.highlightTicks = 0;
        p.id = i;  // Уникальный идентификатор частицы
        particles.push_back(p);
    }
}

void simulate(std::vector<Particle>& particles, int width, int height, bool enableRandomEvents, Statistics& stats) {
    const float friction = 0.1f;
    const float baseSpeedFactor = 0.1f;

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);
    std::uniform_int_distribution<int> distType(0, TYPE_COUNT - 1);
    std::uniform_real_distribution<float> distMass(1.0f, 1.5f);
    std::uniform_real_distribution<float> distShift(-1.0f, 1.0f);
    std::uniform_int_distribution<int> distEvent(0, 6);

    for (size_t i = 0; i < particles.size(); ++i) {
        auto& p = particles[i];

        // Случайные события
        if (enableRandomEvents) {
            float eventChance = 0.01f;
            if (distProb(rng) < eventChance) {
                int eventType = distEvent(rng);
                stats.totalRandomEvents++;
                stats.recordRandomEvent(p.id);

                switch (eventType) {
                    case 0:
                        stats.removedParticles++;
                        particles.erase(particles.begin() + i);
                        --i;
                        continue;
                    case 1:
                        stats.typeChanges++;
                        p.type = distType(rng);
                        p.highlightTicks = 5;
                        break;
                    case 2: {
                        stats.reproductions++;
                        Particle child = p;
                        child.x += distShift(rng);
                        child.y += distShift(rng);
                        child.mass = distMass(rng);
                        child.vx = 0.0f;
                        child.vy = 0.0f;
                        child.highlightTicks = 5;
                        child.id = static_cast<int>(particles.size());
                        particles.push_back(child);
                        p.highlightTicks = 5;
                        break;
                    }
                    case 3:
                        stats.teleports++;
                        p.x = distProb(rng) * width;
                        p.y = distProb(rng) * height;
                        p.highlightTicks = 5;
                        break;
                    case 4:
                        stats.massChanges++;
                        p.mass = distMass(rng);
                        p.highlightTicks = 5;
                        break;
                    case 5:
                        stats.speedJumps++;
                        p.vx = distShift(rng) * 2.0f;
                        p.vy = distShift(rng) * 2.0f;
                        p.highlightTicks = 5;
                        break;
                    case 6:
                        stats.sleepingParticles++;
                        p.vx = 0.0f;
                        p.vy = 0.0f;
                        p.highlightTicks = 5;
                        break;
                }
            }
        }

        float ax = 0.0f;
        float ay = 0.0f;
        for (const auto& other : particles) {
            if (&p == &other) continue;

            float dx = other.x - p.x;
            float dy = other.y - p.y;

            // Торроидальное (периодическое) пространство
            if (dx > width / 2) dx -= width;
            else if (dx < -width / 2) dx += width;
            if (dy > height / 2) dy -= height;
            else if (dy < -height / 2) dy += height;

            float dist_sq = dx * dx + dy * dy + 0.01f; // Смещение для предотвращения деления на 0
            float dist = std::sqrt(dist_sq);

            int t1 = p.type;
            int t2 = other.type;
            float force = interactionMatrix[t1][t2];
            float accel = force * other.mass / dist_sq;

            ax += accel * dx / dist;
            ay += accel * dy / dist;
        }

        p.vx += ax * baseSpeedFactor;
        p.vy += ay * baseSpeedFactor;

        p.vx *= (1.0f - friction);
        p.vy *= (1.0f - friction);

        p.x += p.vx;
        p.y += p.vy;

        // Обеспечение цикличности по краям
        if (p.x < 0) p.x += width;
        if (p.x >= width) p.x -= width;
        if (p.y < 0) p.y += height;
        if (p.y >= height) p.y -= height;

        if (p.highlightTicks > 0)
            p.highlightTicks--;
    }
}


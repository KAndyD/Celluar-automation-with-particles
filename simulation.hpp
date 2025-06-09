#pragma once
#include <vector>
#include "particle.hpp"
#include "statistics.hpp"

// Создаёт count частиц с начальными случайными параметрами и добавляет их в particles
void reset_particles(std::vector<Particle>& particles, int count, int width, int height);
void simulate(std::vector<Particle>& particles, int width, int height, bool enableRandomEvents, Statistics& stats);
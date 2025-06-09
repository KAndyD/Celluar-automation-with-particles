#include "renderer.hpp"
#include "config.hpp"
#include <iostream>

const char typeChars[] = { 'o', '*', '+' };
const char* typeColors[] = {
    "\033[31m",  // red
    "\033[32m",  // green
    "\033[34m"   // blue
};
const char* HIGHLIGHT_COLOR = "\033[1;43m"; // фон
const char* RESET_COLOR = "\033[0m";

void render(const std::vector<Particle>& particles, int width, int height) {
    const int GRID_WIDTH = width;
    const int GRID_HEIGHT = height;
    char grid[GRID_HEIGHT][GRID_WIDTH];
    int type[GRID_HEIGHT][GRID_WIDTH];
    const Particle* owner[GRID_HEIGHT][GRID_WIDTH];

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            grid[y][x] = ' ';
            type[y][x] = -1;
            owner[y][x] = nullptr;
        }
    }

    for (const auto& p : particles) {
        int gx = static_cast<int>(p.x);
        int gy = static_cast<int>(p.y);
        if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT) {
            grid[gy][gx] = typeChars[p.type % 3];
            type[gy][gx] = p.type % 3;
            owner[gy][gx] = &p;
        }
    }

    // Отрисовка в консоли
    std::cout << "\033[2J\033[1;1H";
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (type[y][x] != -1 && owner[y][x]) {
                if (owner[y][x]->highlightTicks > 0)
                    std::cout << HIGHLIGHT_COLOR << typeColors[type[y][x]] << grid[y][x] << RESET_COLOR;
                else
                    std::cout << typeColors[type[y][x]] << grid[y][x] << RESET_COLOR;
            } else {
                std::cout << ' ';
            }
        }
        std::cout << '\n';
    }
    std::cout.flush();
}

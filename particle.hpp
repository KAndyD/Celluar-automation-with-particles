#ifndef PARTICLE_HPP
#define PARTICLE_HPP

struct Particle {
    float x, y;     // позиция
    float vx, vy;   // скорость
    int type;       // тип частицы
    float mass;     // масса частицы
    int highlightTicks = 0; // сколько кадров подсвечивать
    int id;         // идентификатор частицы
};

#endif // PARTICLE_HPP
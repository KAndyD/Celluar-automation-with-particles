#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>
#include "particle.hpp"
#include <cstddef>

struct Statistics {
    int removedParticles = 0; // Счётчик удалённых частиц
    int reproductions = 0; // Счётчик событий размножения частиц
    int typeChanges = 0; // Счётчик смены типа частицы
    int teleports = 0; // Счётчик телепортаций
    int sleepingParticles = 0; // Количество "спящих" частиц
    int massChanges = 0; // Счётчик изменений массы у частиц
    int speedJumps = 0; // Счётчик резких скачков скорости
    int totalRandomEvents = 0; // Общее количество случайных событий, произошедших в симуляции
    int particlesWithEvents = 0; // Количество частиц, с которыми хотя бы один раз произошло случайное событие

    size_t simulationSteps = 0; // Общее количество шагов симуляции (итераций основного цикла)
    size_t totalParticleCount = 0; // Последнее известное количество частиц (используется при выводе итогов)

    // Топ частицы по массе (индексы и массы)
    std::vector<std::pair<double, size_t>> topMassiveParticles;

    // Булевый вектор по числу частиц — для отслеживания, была ли хотя бы раз с данной частицей случайность
    std::vector<bool> hadRandomEvent;

    void reset(size_t particleCount); // Сброс всех статистических данных
    void recordRandomEvent(size_t particleIndex); // Фиксация того, что с конкретной частицей (по индексу) произошло случайное событие
    void incrementEventCount(int eventType); // Универсальный метод для увеличения счётчиков по типу события
    void incrementRemoved(); // Увеличение счётчика удалённых частиц
    void incrementStep(); // Увеличение количества шагов симуляции
    void updateParticleCount(size_t count); // Обновление общего количества частиц(если меняется)

    void printSummary(const std::vector<Particle>& particles); // Печать краткой сводной статистики симуляции в консоль
    void saveToCSV(const std::vector<Particle>& particles, const char* filename); // Сохранение данных о симуляции и частицах в файл .csv
}; 

#endif // STATISTICS_HPP

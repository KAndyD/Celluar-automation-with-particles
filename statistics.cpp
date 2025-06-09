#include "statistics.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>
#include <iomanip>

// --- Цветовые коды ANSI для вывода в консоли ---
#define ANSI_RED     "\033[1;31m"
#define ANSI_GREEN   "\033[1;32m"
#define ANSI_BLUE    "\033[1;34m"
#define ANSI_RESET   "\033[0m"

// Возвращает строку с цветом и буквой, соответствующей типу частицы
std::string typeColored(int type) {
    switch(type) {
        case 0: return std::string(ANSI_RED) + "0(R)" + ANSI_RESET;
        case 1: return std::string(ANSI_GREEN) + "1(G)" + ANSI_RESET;
        case 2: return std::string(ANSI_BLUE) + "2(B)" + ANSI_RESET;
        default: return std::to_string(type);
    }
}

// Сброс всей накопленной статистики перед новой симуляцией
void Statistics::reset(size_t particleCount) {
    removedParticles = 0;
    reproductions = 0;
    typeChanges = 0;
    teleports = 0;
    sleepingParticles = 0;
    massChanges = 0;
    speedJumps = 0;
    totalRandomEvents = 0;
    particlesWithEvents = 0;
    simulationSteps = 0;
    totalParticleCount = 0;
    hadRandomEvent.assign(particleCount, false);  // флаг событий на каждую частицу
    topMassiveParticles.clear();
}

// Учитывает событие, произошедшее с конкретной частицей (уникальное по индексу)
void Statistics::recordRandomEvent(size_t particleIndex) {
    if (particleIndex < hadRandomEvent.size() && !hadRandomEvent[particleIndex]) {
        hadRandomEvent[particleIndex] = true;
        particlesWithEvents++;
    }
    totalRandomEvents++;
}

// Увеличивает счётчик события по типу (enum-like)
void Statistics::incrementEventCount(int eventType) {
    switch (eventType) {
        case 0: incrementRemoved(); break;
        case 1: typeChanges++; break;
        case 2: reproductions++; break;
        case 3: teleports++; break;
        case 4: massChanges++; break;
        case 5: speedJumps++; break;
        case 6: sleepingParticles++; break;
    }
}

// Увеличивает счётчик удалённых частиц
void Statistics::incrementRemoved() {
    removedParticles++;
}

// Увеличивает счётчик шагов симуляции
void Statistics::incrementStep() {
    simulationSteps++;
}

// Суммирует общее количество частиц, появившихся за время симуляции
void Statistics::updateParticleCount(size_t count) {
    totalParticleCount += count;
}

// Вычисляет евклидово расстояние между двумя точками
static double dist(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx*dx + dy*dy);
}

// Основная функция — печать всей статистики симуляции
void Statistics::printSummary(const std::vector<Particle>& particles) {
    using namespace std;

    size_t count = particles.size();
    if (count == 0) {
        cout << "Нет частиц для статистики.\n";
        return;
    }

    cout << "\n--- Итоговая статистика симуляции ---\n";

    // --- Подсчёт количества и массы по типам ---
    map<int, int> countByType;
    map<int, double> massSumByType;
    for (const auto& p : particles) {
        countByType[p.type]++;
        massSumByType[p.type] += p.mass;
    }

    cout << "Количество частиц по типам:\n";
    for (const auto& [type, cnt] : countByType) {
        cout << "  Тип " << typeColored(type) << ": " << cnt << '\n';
    }

    // --- Средняя масса по типам и в целом ---
    cout << "Средняя масса по типам:\n";
    double totalMass = 0.0;
    for (const auto& [type, cnt] : countByType) {
        double avgMass = massSumByType[type] / cnt;
        cout << "  Тип " << typeColored(type) << ": " << avgMass << '\n';
        totalMass += massSumByType[type];
    }
    cout << "Средняя масса всех частиц: " << totalMass / count << '\n';

    // --- Подсчёт событий ---
    cout << "Количество удалённых частиц (взрывов): " << removedParticles << '\n';
    cout << "Количество размножений: " << reproductions << '\n';
    cout << "Количество смен типов: " << typeChanges << '\n';
    cout << "Количество телепортаций: " << teleports << '\n';
    cout << "Количество 'спящих' частиц: " << sleepingParticles << '\n';
    cout << "Количество смен массы: " << massChanges << '\n';
    cout << "Количество резких ускорений: " << speedJumps << '\n';
    cout << "Всего случайных событий: " << totalRandomEvents << '\n';
    cout << "Частиц, переживших ≥1 случайное событие: " << particlesWithEvents << '\n';

    // --- Геометрия: дисперсия, расстояния ---
    vector<double> xs, ys;
    xs.reserve(count);
    ys.reserve(count);
    for (const auto& p : particles) {
        xs.push_back(p.x);
        ys.push_back(p.y);
    }

    double meanX = accumulate(xs.begin(), xs.end(), 0.0) / count;
    double meanY = accumulate(ys.begin(), ys.end(), 0.0) / count;

    double varianceX = 0.0, varianceY = 0.0;
    for (size_t i = 0; i < count; ++i) {
        varianceX += (xs[i] - meanX) * (xs[i] - meanX);
        varianceY += (ys[i] - meanY) * (ys[i] - meanY);
    }
    varianceX /= count;
    varianceY /= count;

    double stdDevX = sqrt(varianceX);
    double stdDevY = sqrt(varianceY);

    // --- Расстояния между всеми и однотипными ---
    double distSumAll = 0.0;
    size_t distCountAll = 0;
    map<int, double> distSumByType;
    map<int, size_t> distCountByType;

    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            double d = dist(particles[i].x, particles[i].y, particles[j].x, particles[j].y);
            distSumAll += d;
            distCountAll++;
            if (particles[i].type == particles[j].type) {
                distSumByType[particles[i].type] += d;
                distCountByType[particles[i].type]++;
            }
        }
    }

    cout << fixed << setprecision(3);
    cout << "Среднее расстояние между всеми частицами: " 
         << (distCountAll ? distSumAll / distCountAll : 0.0) << '\n';

    cout << "Среднее расстояние между частицами одного типа:\n";
    for (const auto& [type, sumD] : distSumByType) {
        double avgD = distCountByType[type] ? sumD / distCountByType[type] : 0.0;
        cout << "  Тип " << typeColored(type) << ": " << avgD << '\n';
    }

    // --- Плотность на площади ---
    double minX = *min_element(xs.begin(), xs.end());
    double maxX = *max_element(xs.begin(), xs.end());
    double minY = *min_element(ys.begin(), ys.end());
    double maxY = *max_element(ys.begin(), ys.end());
    double area = (maxX - minX) * (maxY - minY);
    if (area < 0.01) area = 1;
    double density = count / area;
    cout << "Средняя плотность (частиц на единицу площади): " << density << '\n';
    cout << "Стандартное отклонение X: " << stdDevX << ", Y: " << stdDevY << '\n';

    // --- Скорости частиц ---
    vector<double> speeds;
    speeds.reserve(count);
    map<int, vector<double>> speedsByType;

    for (const auto& p : particles) {
        double sp = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        speeds.push_back(sp);
        speedsByType[p.type].push_back(sp);
    }

    double avgSpeed = std::accumulate(speeds.begin(), speeds.end(), 0.0) / count;

    cout << "Средняя скорость всех частиц: " << avgSpeed << '\n';
    cout << "Средняя скорость по типам:\n";
    for (const auto& [type, spVec] : speedsByType) {
        double sumSp = std::accumulate(spVec.begin(), spVec.end(), 0.0);
        cout << "  Тип " << typeColored(type) << ": " << sumSp / spVec.size() << '\n';
    }

    // --- Топ 3 по скорости и массе ---
    vector<std::pair<double, size_t>> speedIndex, massIndex;
    for (size_t i = 0; i < count; ++i) {
        speedIndex.emplace_back(speeds[i], i);
        massIndex.emplace_back(particles[i].mass, i);
    }

    sort(speedIndex.begin(), speedIndex.end(), [](auto& a, auto& b) { return a.first > b.first; });
    sort(massIndex.begin(), massIndex.end(), [](auto& a, auto& b) { return a.first > b.first; });

    cout << "Частицы с наибольшей скоростью (топ 3):\n";
    for (int i = 0; i < 3 && i < (int)speedIndex.size(); ++i) {
        size_t idx = speedIndex[i].second;
        cout << "  Индекс " << idx << " Скорость: " << speedIndex[i].first 
             << " Тип: " << typeColored(particles[idx].type) << '\n';
    }

    cout << "Частицы с наименьшей скоростью (топ 3):\n";
    for (int i = 0; i < 3 && i < (int)speedIndex.size(); ++i) {
        size_t idx = speedIndex[speedIndex.size() - 1 - i].second;
        cout << "  Индекс " << idx << " Скорость: " << speeds[idx] 
             << " Тип: " << typeColored(particles[idx].type) << '\n';
    }

    cout << "Частицы с наибольшей массой (топ 3):\n";
    for (int i = 0; i < 3 && i < (int)massIndex.size(); ++i) {
        size_t idx = massIndex[i].second;
        cout << "  Индекс " << idx << " Масса: " << massIndex[i].first 
             << " Тип: " << typeColored(particles[idx].type) << '\n';
    }

    // --- Общая статистика ---
    cout << "Число шагов симуляции: " << simulationSteps << '\n';
    double avgParticlesPerFrame = simulationSteps ? 
        (double)totalParticleCount / simulationSteps : count;
    cout << "Среднее количество частиц на кадр: " << avgParticlesPerFrame << '\n';

    // --- Сближения ---
    int closePairs = 0;
    for (size_t i = 0; i < count; ++i)
        for (size_t j = i + 1; j < count; ++j)
            if (dist(particles[i].x, particles[i].y, particles[j].x, particles[j].y) < 2.0)
                closePairs++;
    cout << "Количество близких сближений (<2.0): " << closePairs << '\n';

    // --- Сетка плотности 10x10 ---
    const int GRID_SIZE = 10;
    std::vector<int> cellCounts(GRID_SIZE * GRID_SIZE, 0);
    for (const auto& p : particles) {
        int cx = static_cast<int>(((p.x - minX) / (maxX - minX)) * GRID_SIZE);
        int cy = static_cast<int>(((p.y - minY) / (maxY - minY)) * GRID_SIZE);
        cx = std::clamp(cx, 0, GRID_SIZE - 1);
        cy = std::clamp(cy, 0, GRID_SIZE - 1);
        cellCounts[cy * GRID_SIZE + cx]++;
    }
    int maxDensityCell = *std::max_element(cellCounts.begin(), cellCounts.end());
    cout << "Максимальная плотность в одной ячейке (10x10 сетка): " << maxDensityCell << '\n';

    cout << "--- Конец статистики ---\n\n";
}

// Сохраняет краткую статистику в CSV-файл
void Statistics::saveToCSV(const std::vector<Particle>& particles, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла для записи статистики: " << filename << '\n';
        return;
    }

    file << "Тип,Количество,Средняя масса,Средняя скорость\n";

    std::map<int, int> countByType;
    std::map<int, double> massSumByType;
    std::map<int, double> speedSumByType;

    for (const auto& p : particles) {
        countByType[p.type]++;
        massSumByType[p.type] += p.mass;
        double sp = std::sqrt(p.vx * p.vx + p.vy * p.vy);
        speedSumByType[p.type] += sp;
    }

    for (const auto& [type, cnt] : countByType) {
        double avgMass = massSumByType[type] / cnt;
        double avgSpeed = speedSumByType[type] / cnt;
        file << type << "," << cnt << "," << avgMass << "," << avgSpeed << "\n";
    }

    file << "\nОбщие события,Значения\n";
    file << "Удалённых частиц (взрывов)," << removedParticles << "\n";
    file << "Размножений," << reproductions << "\n";
    file << "Смен типов," << typeChanges << "\n";
    file << "Телепортаций," << teleports << "\n";
    file << "Спящих частиц," << sleepingParticles << "\n";
    file << "Смен массы," << massChanges << "\n";
    file << "Резких ускорений," << speedJumps << "\n";
    file << "Всего случайных событий," << totalRandomEvents << "\n";
    file << "Частиц с событиями," << particlesWithEvents << "\n";
    file << "Шагов симуляции," << simulationSteps << "\n";
}

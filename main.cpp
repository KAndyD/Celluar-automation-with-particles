#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <array>
#include "particle.hpp"
#include "simulation.hpp"
#include "renderer.hpp"
#include "config.hpp"
#include "statistics.hpp"
#include "group.hpp"

std::array<std::array<float, TYPE_COUNT>, TYPE_COUNT> interactionMatrix;

// Неблокирующая проверка нажатия клавиш: Настраивает терминал на неблокирующий ввод; 
// Проверяет наличие символа в буфере ввода;
// Восстанавливает настройки терминала;
// Возвращает 1, если клавиша нажата, и 0 в противном случае
int kbhit() {
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}


int main() {
    std::srand(std::time(nullptr));

    int particleCount = 0;
    do {
        std::cout << "Введите количество частиц (рекомендуется 50 - 500): ";
        std::cin >> particleCount;
    } while (particleCount <= 0);

    int preset = 0;
    std::cout << "Выберите тип взаимодействия (пресет):\n";
    std::cout << "1 - Охота (догонялки)\n";
    std::cout << "2 - Расслоение (разные типы избегают друг друга)\n";
    std::cout << "3 - Хаос (рандомные взаимодействия частиц)\n";
    std::cout << "4 - Союзы (2 типа объединяются против третьего)\n";
    std::cout << "5 - Группировка (частицы одинаковых типов создают несколько плотных кластеров - шаблон для будущих пресетов фигур)\n"; 
    do {
        std::cout << "Введите номер пресета (1-5): ";
        std::cin >> preset;
    } while (preset < 1 || preset > 5);

    if (preset != 5) {
        interactionMatrix = getInteractionMatrix(preset);
    }

    char ch;
    bool enableRandomEvents = false;
    do {
        std::cout << "Включить случайные события? (y/n): ";
        std::cin >> ch;
        ch = tolower(ch);
    } while (ch != 'y' && ch != 'n');
    enableRandomEvents = (ch == 'y');

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int termWidth = w.ws_col;
    int termHeight = w.ws_row;

    std::vector<Particle> particles;

    Statistics stats;
    stats.reset(particleCount);

    if (preset == 5) {
        init_group(particles, particleCount, termWidth, termHeight);
    } else {
        reset_particles(particles, particleCount, termWidth, termHeight);
    }

    while (true) {
        if (kbhit()) {
            char input = getchar();
            if (input == 'q') break;
            if (input == 'r') {
                if (preset == 5) {
                    init_group(particles, particleCount, termWidth, termHeight);
                } else {
                    reset_particles(particles, particleCount, termWidth, termHeight);
                }
                stats.reset(particleCount);
            }
        }

        if (preset == 5) {
            update_group(particles, termWidth, termHeight);
        } else {
            simulate(particles, termWidth, termHeight, enableRandomEvents, stats);
        }
        stats.incrementStep();
        stats.updateParticleCount(particles.size());

        render(particles, termWidth, termHeight);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    stats.printSummary(particles);
    stats.saveToCSV(particles, "statistics.csv");

    return 0;
}